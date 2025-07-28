#include "SpawningSystem/Spawners/EnemySpawner.h"

#if WITH_EDITOR
#include "Kismet/GameplayStatics.h"
#endif

#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "PoolingSystem/Actors/Pool.h"
#include "SpawningSystem/Data/EnemyPhaseData.h"
#include "SpawningSystem/Data/SpawnerConfig.h"
#include "SpawningSystem/Utils/SpawnLocation.h"

FOnPhaseCompleted AEnemySpawner::OnPhaseCompleted;

AEnemySpawner::AEnemySpawner()
{

#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
#else
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
#endif
	InitialSpawnDelay = 0.0f;

#if WITH_EDITOR
	bDrawDebugInfo = false;
#endif
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	checkf(SpawnerConfig, TEXT("SpawnerConfig on [%s] is not set, make sure to set it!"), *GetNameSafe(this));
	checkf(SpawnLocations.Num() != 0, TEXT("There are no Spawn Locations set on [%s], make sure to set them!"),
		*GetNameSafe(this));

	if (!SpawnerConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Spawner config, the system won't run."));
		return;
	}

	const UWorld* World = GetWorld();

	if (!World) return;

	for (APool* Pool : Pools)
	{
		if (!IsValid(Pool)) continue;

		TSubclassOf<AActor> PoolClass = Pool->GetObjectToSpawn();
		if (!IsValid(PoolClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("ObjectToSpawn not valid in a pool!"));
			continue;
		}

		if (!PoolMap.Contains(PoolClass))
		{
			PoolMap.Add(PoolClass, Pool);
			Pool->Init();
		}
	}

	if (PoolMap.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No valid pools found in SpawnerConfig!"));
		return;
	}

	// Setup initial spawn state
	SpawnerState.CurrentSpawnProb = SpawnerConfig->SpawnProb;
	SpawnerState.CurrentGroupSize = SpawnerConfig->MinEnemyGroupNumber;
	SpawnerState.CurrentPhase = SpawnerConfig->Phases[0].Phase;

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(World))
	{
		NPCSubsystem->OnEnemyDied.AddDynamic(this, &ThisClass::OnEnemyKilled);
		NPCSubsystem->OnStopAllEnemies.AddDynamic(this, &ThisClass::StopSpawner);
	}

	if (bAutoStart)
	{
		SpawnerState.bCanSpawn = true;
		StartSpawner();
	}
}

void AEnemySpawner::StartSpawner()
{
	bIsManuallyStopped = false;
	SpawnerState.bCanSpawn = true;
	UE_LOG(LogTemp, Display, TEXT("Starting Spawner [%s]"), *GetName());

	GetWorldTimerManager().SetTimer(
		SpawnerTimerHandle,
		this,
		&AEnemySpawner::TrySpawnGroup,
		SpawnerState.CurrentPhase->Delay,
		true,
		InitialSpawnDelay);
}

void AEnemySpawner::StopSpawner()
{
	bIsManuallyStopped = true;
	SpawnerState.bCanSpawn = false;
	GetWorldTimerManager().ClearTimer(SpawnerTimerHandle);
	UE_LOG(LogTemp, Log, TEXT("[%s] has stopped!"), *GetName());
}

void AEnemySpawner::TrySpawnGroup()
{
	// Spawner is stopped/paused
	if (!SpawnerState.bCanSpawn || bIsManuallyStopped) return;

	// Phase is null
	if (!SpawnerState.CurrentPhase) return;

	const int32 MaxEnemies = SpawnerState.CurrentPhase->MaxEnemiesInArena;
	const int32 AvailableSpawningSlots = FMath::Max(0, MaxEnemies - SpawnerState.AliveEnemies);

	// No more room in the arena
	if (AvailableSpawningSlots == 0) return;

	const int32 CurrentGroupSize = SpawnerState.CurrentGroupSize;

	// The group is somehow larger that the amount of enemies that can be in the arena
	if (CurrentGroupSize > MaxEnemies) return;

	const int32 GroupCap = SpawnerConfig->EnemyGroupCap;
	const float RandProb = FMath::RandRange(0.0f, 100.0f);

	// Random throw has met spawn probability or group size reached cap
	const bool bShouldSpawn = SpawnerState.CurrentSpawnProb >= RandProb || CurrentGroupSize == GroupCap;

	if (bShouldSpawn)
	{
		SpawnGroup(FMath::Min(CurrentGroupSize, AvailableSpawningSlots));
		SpawnerState.CurrentGroupSize = SpawnerConfig->MinEnemyGroupNumber;
		SpawnerState.CurrentSpawnProb = SpawnerConfig->SpawnProb;
	}
	else
	{
		if (CurrentGroupSize > AvailableSpawningSlots) return;

		SpawnerState.CurrentSpawnProb += SpawnerConfig->SpawnPercentDelta;

		if (CurrentGroupSize < AvailableSpawningSlots)
		{
			++SpawnerState.CurrentGroupSize;
			SpawnerState.CurrentGroupSize = FMath::Clamp(SpawnerState.CurrentGroupSize,
				SpawnerConfig->MinEnemyGroupNumber,
				SpawnerConfig->EnemyGroupCap);
		}
	}
}

void AEnemySpawner::SpawnGroup(const int32 Amount)
{
	if (Amount <= 0) return;

	CachedSpawnPoint = GetRandomSpawnPoint();
	RemainingEnemiesToSpawn = Amount;

	GetWorldTimerManager().SetTimerForNextTick(this, &AEnemySpawner::SpawnSingleEnemy);
}

void AEnemySpawner::SpawnSingleEnemy()
{
	if (RemainingEnemiesToSpawn <= 0) return;

	if (TSubclassOf<AActor> EnemyClass = GetRandomEnemyClass(); IsValid(EnemyClass))
	{
		if (!PoolMap.Contains(EnemyClass)) return;

		if (AActor* EnemyActor = PoolMap[EnemyClass]->TryGetPoolable(); EnemyActor != nullptr && IsValid(EnemyActor))
		{
			Cast<ANpcBase>(EnemyActor)->Spawn(CachedSpawnPoint);
			EnemyActor->SetActorLocation(CachedSpawnPoint);
			SpawnerState.TotalSpawnedEnemies++;
			SpawnerState.AliveEnemies++;

			if (SpawnerState.TotalSpawnedEnemies == SpawnerConfig->TotalEnemiesToSpawn)
			{
				StopSpawner();
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Actor is null, skipping iteration..."));
		}
	}

	RemainingEnemiesToSpawn--;

	if (RemainingEnemiesToSpawn > 0)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AEnemySpawner::SpawnSingleEnemy);
	}
}

TSubclassOf<AActor> AEnemySpawner::GetRandomEnemyClass() const
{
	const TArray<FEnemyWeightEntry>& Weights = SpawnerState.CurrentPhase->EnemyWeights;
	int32 TotalWeight = 0;

	for (const auto& [EnemyClass, Weight] : Weights)
		TotalWeight += Weight;

	TotalWeight = FMath::Clamp(TotalWeight, 1, 100);

	if (TotalWeight <= 0) return nullptr;

	int32 Choice = FMath::RandRange(1, TotalWeight);

	for (const auto& [EnemyClass, Weight] : Weights)
	{
		Choice -= Weight;

		if (Choice <= 0)
			return EnemyClass;
	}
	return nullptr;
}

FVector AEnemySpawner::GetRandomSpawnPoint()
{
	if (SpawnLocations.IsEmpty()) return GetActorLocation();

	int32 Index = FMath::RandRange(0, (SpawnLocations.Num() - 1));

	// Prevent overflow
	if (Index == SpawnerState.LastSpawnPointIndex)
	{
		Index = (Index + 1) % SpawnLocations.Num();
	}

	SpawnerState.LastSpawnPointIndex = Index;
	return SpawnLocations[Index]->GetActorLocation() + GetRandomSpawnDeviation();
}

FORCEINLINE FVector AEnemySpawner::GetRandomSpawnDeviation() const
{
	const float xDev = FMath::RandRange(0.0f, SpawnerConfig->XAxisSpawnDeviation);
	const float yDev = FMath::RandRange(0.0f, SpawnerConfig->YAxisSpawnDeviation);

	return FVector(xDev, yDev, 0.0f);
}

void AEnemySpawner::OnEnemyKilled(const AActor* KilledEnemy, const int32 PointsPerKill, const AActor* Killer)
{
	SpawnerState.AliveEnemies--;
	SpawnerState.PhaseEnemiesKilled++;

	if (!bIsManuallyStopped && SpawnerConfig->TotalEnemiesToSpawn != -1)
	{
		SpawnerState.bCanSpawn = !(SpawnerState.CurrentPhaseIndex == SpawnerConfig->Phases.Num() - 1
			&& SpawnerConfig->Phases[SpawnerState.CurrentPhaseIndex].EnemiesToKill - SpawnerState.PhaseEnemiesKilled <=
			0);
	}

	UpdatePhase();
}

void AEnemySpawner::UpdatePhase()
{
	const int32 NextIndex = SpawnerState.CurrentPhaseIndex + 1;

	if (!SpawnerConfig->Phases.IsValidIndex(NextIndex) ||
		SpawnerConfig->Phases[NextIndex].EnemiesToKill == -1 ||
		SpawnerState.PhaseEnemiesKilled < SpawnerConfig->Phases[NextIndex].EnemiesToKill)
		return;

	SpawnerState.CurrentPhaseIndex = NextIndex;
	SpawnerState.PhaseEnemiesKilled = 0;
	SpawnerState.CurrentPhase = SpawnerConfig->Phases[NextIndex].Phase;

	OnPhaseCompleted.Broadcast(SpawnerConfig->Phases[NextIndex - 1].Phase, SpawnerState.CurrentPhase,
		NextIndex, NextIndex == SpawnerConfig->Phases.Num() - 1);

	if (!SpawnerState.bCanSpawn || bIsManuallyStopped)
	{
		GetWorldTimerManager().ClearTimer(SpawnerTimerHandle);
		return;
	}

	GetWorldTimerManager().ClearTimer(SpawnerTimerHandle);
	GetWorldTimerManager().SetTimer(
		SpawnerTimerHandle,
		this,
		&AEnemySpawner::TrySpawnGroup,
		SpawnerState.CurrentPhase->Delay,
		true,
		SpawnerConfig->DelayBetweenPhases);
	UE_LOG(LogTemp, Display, TEXT("Phase changed to %d"), NextIndex);
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.RemoveDynamic(this, &ThisClass::OnEnemyKilled);
	}
}

// Debug section

#if WITH_EDITOR
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SpawnerConfig || !bDrawDebugInfo) return;
	DebugDraw();
}
#endif

#if WITH_EDITOR
void AEnemySpawner::DebugDraw()
{
	const FVector DebugLocation = GetActorLocation() + FVector(0, 0, 200);
	const int EnemiesToKill = SpawnerConfig->Phases[FMath::Clamp(SpawnerState.CurrentPhaseIndex + 1, 0,
		SpawnerConfig->Phases.Num() - 1)].EnemiesToKill -
		SpawnerState.PhaseEnemiesKilled;


	const FString Text = FString::Printf
	(TEXT("Current Phase: %s\n"
		"Alive Enemies: %d\n"
		"Total Enemies: %d\n"
		"Spawn Prob: %.2f\n"
		"Group Size: %d\n"
		"Enemies to kill: %d\n"
		"Max concurrent enemies for [%s]: %d\n"
		"Can Spawn: [%s]"),
		*SpawnerState.CurrentPhase->GetName(),
		SpawnerState.AliveEnemies,
		SpawnerState.TotalSpawnedEnemies,
		SpawnerState.CurrentSpawnProb,
		SpawnerState.CurrentGroupSize,
		EnemiesToKill,
		*SpawnerState.CurrentPhase->GetName(),
		SpawnerState.CurrentPhase->MaxEnemiesInArena,
		SpawnerState.bCanSpawn ? TEXT("Yes") : TEXT("No")
	);

	DrawDebugString(
		GetWorld(),
		DebugLocation,
		Text,
		this,
		FColor::Yellow,
		0.0f,
		false,
		1.5f
	);
}

void AEnemySpawner::SpawnPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (const auto Pool = World->SpawnActor<APool>(); !Pools.Contains(Pool))
	{
		Pools.Add(Pool);
	}
}

void AEnemySpawner::AutoSetup()
{
	SpawnLocations.Empty();
	Pools.Empty();

	const UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnLocation::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ASpawnLocation* SpawnLocation = Cast<ASpawnLocation>(Actor))
		{
			SpawnLocations.Add(SpawnLocation);
		}
	}

	UGameplayStatics::GetAllActorsOfClass(World, APool::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (APool* Pool = Cast<APool>(Actor))
		{
			Pools.Add(Pool);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d SpawnLocations"), SpawnLocations.Num());
	UE_LOG(LogTemp, Log, TEXT("Found %d Pools"), Pools.Num());
}
#endif
