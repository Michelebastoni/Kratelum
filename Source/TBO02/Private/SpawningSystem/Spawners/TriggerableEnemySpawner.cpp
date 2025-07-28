#include "SpawningSystem/Spawners/TriggerableEnemySpawner.h"

#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "PoolingSystem/Actors/Pool.h"
#include "SpawningSystem/Utils/SpawnLocation.h"

ATriggerableEnemySpawner::ATriggerableEnemySpawner(): XAxisRandomSpawnDeviation(0), YAxisRandomSpawnDeviation(0),
                                                      bAutoTriggerAtThreshold(false),
                                                      TriggerKillThreshold(1), bResetKillCounterAfterTrigger(false),
                                                      KilledEnemies(0)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATriggerableEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!EnemyPools.IsEmpty())
	{
		for (int32 i = 0; i < EnemyPools.Num(); i++)
		{
			// Generate consistent runtime pool cache
			if (!EnemiesGroupsMap.Contains(EnemyPools[i]->GetObjectToSpawn()))
			{
				EnemiesGroupsMap.Add(EnemyPools[i]->GetObjectToSpawn(), EnemyPools[i]);
				EnemyPools[i]->Init(); // Initialize each pool
			}
		}
	}

	TriggerKillThreshold = FMath::Max(TriggerKillThreshold, 1);

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.AddDynamic(this, &ThisClass::OnEnemyDied);
		NPCSubsystem->OnStopAllEnemies.AddDynamic(this, &ThisClass::OnStopSpawner);
	}
}

void ATriggerableEnemySpawner::SpawnGroups(const FTransform& SpawnLocation)
{
	if (!bCanSpawn)
	{
		return;
	}
	
	// Iterate over groups dictionary
	for (const auto& [EnemyClass, Amount] : EnemiesGroups)
	{
		for (int i = 0; i < Amount; i++)
		{
			// Pick from pool
			if (AActor* Enemy = EnemiesGroupsMap[EnemyClass]->TryGetPoolable())
			{
				Enemy->SetActorLocation(GetRandomSpawnLocation(SpawnLocation));
			}
		}
	}
}

void ATriggerableEnemySpawner::SpawnSingle(const TSubclassOf<ANpcBase> NpcClass, const FTransform& SpawnLocation)
{
	if (!bCanSpawn || !IsValid(NpcClass) || !EnemiesGroupsMap.Contains(NpcClass))
	{
		return;
	}

	// Pick from pool
	if (AActor* Enemy = EnemiesGroupsMap[NpcClass]->TryGetPoolable())
	{
		Enemy->SetActorLocation(GetRandomSpawnLocation(SpawnLocation));
	}
}

void ATriggerableEnemySpawner::OnEnemyDied(const AActor* KilledEnemy, const int32 PointsPerKill, const AActor* Killer)
{
	if (!bAutoTriggerAtThreshold)
	{
		return;
	}

	KilledEnemies++;

	if (KilledEnemies != TriggerKillThreshold)
	{
		return; // The kill threshold is not reached or was already met.
	}

	if (bSpawnSingle)
	{
		SpawnSingle(EnemyToSpawn, DefaultSpawnLocation->GetActorTransform());
	}
	else
	{
		SpawnGroups(DefaultSpawnLocation->GetActorTransform());
	}

	if (bResetKillCounterAfterTrigger)
	{
		KilledEnemies = 0; // Reset trigger logic
	}
}

FVector ATriggerableEnemySpawner::GetRandomSpawnLocation(const FTransform& SpawnLocation) const
{
	const float XDev = FMath::RandRange(0.0f, XAxisRandomSpawnDeviation);
	const float YDev = FMath::RandRange(0.0f, YAxisRandomSpawnDeviation);

	return FVector(XDev, YDev, 0.0f) + SpawnLocation.GetLocation();
}

void ATriggerableEnemySpawner::OnStopSpawner()
{
	bCanSpawn = false;
}

void ATriggerableEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.RemoveDynamic(this, &ThisClass::OnEnemyDied);
	}
}
