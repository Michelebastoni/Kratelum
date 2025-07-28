#include "NPCs/Characters/NpcBase.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "IntegritySystem/Integrity.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "PoolingSystem/Actors/Pool.h"
#include "Weapons/Weapon.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "Sound/SoundCue.h"
#include "Traps/Trap.h"
#include "VFXSystem/Actors/EnemiesVfxMultiPool.h"

ANpcBase::ANpcBase(): bForceAutoSetupData(false), PointsPerKill(0), BaseMovementSpeed(0), TimeBetweenAttacks(0),
                      AttackRangeSqr(0),
                      TurnAcceleration(0),
                      MovementAcceleration(0), MaxTurnRate(0),
                      MinFacingTolerance(0),
                      bIsPooled(false), Pool(nullptr), VfxPool(nullptr)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CapsulePtr->SetCollisionObjectType(ECC_GameTraceChannel15); // Enemy Trace Channel
	MeshPtr->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	MovCompPtr = GetCharacterMovement();
}

void ANpcBase::BeginPlay()
{
	Super::BeginPlay();

	// Legacy fallback
	integrityComponent->FullRestore();

	NpcController = Cast<ANpcControllerBase>(GetController());

	NpcSubsystem = UNPCSubsystem::GetInstance(GetWorld());

	if (bForceAutoSetupData)
	{
		SetupFromConfig();
	}

	NpcSubsystem->OnStopAllEnemies.AddDynamic(this, &ThisClass::OnEnemyHasToStop);
}

void ANpcBase::SetupFromConfig()
{
	checkf(EnemyData, TEXT("EnemyData field was not set on NPC!"));
	checkf(EnemyData->AttackRate > 0, TEXT("AttackRate is less than or equal to 0! Cannot perform division by zero!"));

	integrityComponent->SetMaxIntegrityValue(EnemyData->HealthPoints);

	PointsPerKill = EnemyData->KillPoints;
	BaseMovementSpeed = EnemyData->BaseMovementSpeed * 100.0f;
	MovCompPtr->MaxWalkSpeed = BaseMovementSpeed;

	// Special Movement data setup
	MovementAcceleration = EnemyData->AccelerationSpeed * 100.0f;

	// Rotation settings
	MovCompPtr->MaxAcceleration = MovementAcceleration;
	TurnAcceleration = EnemyData->TurnAcceleration;

	// Tolerance for BT Tasks
	MaxTurnRate = EnemyData->MaxTurnRate;
	MinFacingTolerance = EnemyData->MinFacingTolerance;

	TimeBetweenAttacks = 1.0f / EnemyData->AttackRate;
	AttackRangeSqr = EnemyData->AttackRange * EnemyData->AttackRange;

	if (!EnemyData->DropStatEntries.IsEmpty())
	{
		LootTable = EnemyData->DropStatEntries;
	}

	TArray<FSoftObjectPath> CosmeticEffectsPaths;

	if (!EnemyData->DeathSound.IsNull())
	{
		CosmeticEffectsPaths.Add(EnemyData->DeathSound.ToSoftObjectPath());
	}
	if (!EnemyData->HitSound.IsNull())
	{
		CosmeticEffectsPaths.Add(EnemyData->HitSound.ToSoftObjectPath());
	}

	if (CosmeticEffectsPaths.Num() > 0)
	{
		FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

		StreamManager.RequestAsyncLoad(CosmeticEffectsPaths,
		                               FStreamableDelegate::CreateUObject(this, &ANpcBase::OnCosmeticsLoaded));
	}
}

void ANpcBase::ToggleAI(const bool bValue)
{
	if (!NpcController) return;

	NpcController->ToggleBrain(bValue);
	NpcController->GetPathFollowingComponent()->SetComponentTickEnabled(bValue);
}

void ANpcBase::Spawn(const FVector& SpawnPos)
{
	SetActorLocation(SpawnPos);

	if (TryCacheVfxPool())
	{
		VfxPool->TrySpawnVfx(GetSpawnVfxPtr(), SpawnPos);
	}
}

void ANpcBase::OnCosmeticsLoaded()
{
	if (USoundCue* DeathSound = EnemyData->DeathSound.Get())
	{
		DeathSoundCue = DeathSound;
	}
	if (USoundCue* HitSound = EnemyData->HitSound.Get())
	{
		HitSoundCue = HitSound;
	}
}

void ANpcBase::Attack(AActor* Target)
{
}

void ANpcBase::UpdateRotation(const float DeltaTime, const AActor* Target)
{
	if (!NpcController || !Target) return;

	const FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).
		GetSafeNormal2D();
	if (ToTarget.IsNearlyZero()) return;

	const float TargetYaw = ToTarget.Rotation().Yaw;
	const float CurrentYaw = GetActorRotation().Yaw;
	const float YawDelta = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

	if (FMath::Abs(YawDelta) < MinFacingTolerance) return;

	const float DesiredYawSpeed = FMath::Clamp(YawDelta / DeltaTime, -MaxTurnRate, MaxTurnRate);
	CurrentRotationSpeed.Yaw = FMath::FInterpTo(CurrentRotationSpeed.Yaw, DesiredYawSpeed, DeltaTime, TurnAcceleration);

	const float YawStep = CurrentRotationSpeed.Yaw * DeltaTime;
	const float NewYaw = FMath::UnwindDegrees(CurrentYaw + YawStep);

	SetActorRotation(FRotator(0.f, NewYaw, 0.f));
}

void ANpcBase::AutoReturnToPool()
{
	if (!Pool)
	{
		Destroy();
		return;
	}

	Pool->ReturnPoolable(this);

	if (PoolTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(PoolTimerHandle);
	}
}

void ANpcBase::HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue,
                                       const float currentIntegrityValue, const float changedValue,
                                       const float integrityValueRate, AActor* invader)
{
	if (!invader)
	{
		return;
	}

	if (invader->IsA(ANpcBase::StaticClass())) return;

	if (HitSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSoundCue.Get(), GetActorLocation(), FRotator::ZeroRotator);
	}

	if (!TryCacheVfxPool())
	{
		return;
	}

	VfxPool->TrySpawnVfx(GetHitVfxPtr(), GetActorLocation());

	Super::HandleOnChangeIntegrity(integrityComp, previousIntegrityValue, currentIntegrityValue, changedValue,
	                               integrityValueRate, invader);
}

void ANpcBase::HandleCharacterDeath(const AActor* character, const AActor* harmer, const UIntegrity* component,
                                    const float currentIntegrity)
{
	if (!bIsAlive) return;

	SpawnDeathVfx(harmer);

	Super::HandleCharacterDeath(character, harmer, component, currentIntegrity);

	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DeathSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundCue.Get(), GetActorLocation(),
		                                      FRotator::ZeroRotator);
	}

	ToggleAI(false);

	DropLoot();

	NpcSubsystem->OnEnemyDied.Broadcast(character, PointsPerKill, harmer);

	GetWorldTimerManager().SetTimer(
		PoolTimerHandle,
		this,
		&ANpcBase::AutoReturnToPool,
		TimeToReturnToPool);
}

void ANpcBase::SpawnDeathVfx(const AActor* Killer)
{
	if (!Killer)
	{
		return;
	}

	if (!TryCacheVfxPool())
	{
		return;
	}

	if (Killer->IsA(ATrap::StaticClass()))
	{
		VfxPool->TrySpawnVfx(GetTrapDeathVfxPtr(), GetActorLocation());
	}
	else
	{
		VfxPool->TrySpawnVfx(GetDeathVfxPtr(), GetActorLocation());
	}
}

void ANpcBase::ToggleCapsuleCollision()
{
	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsulePtr->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);

	for (const auto& [Channel, Response] : ActiveCollisionChannelResponses)
	{
		CapsulePtr->SetCollisionResponseToChannel(Channel, Response);
	}
}

// Pool implementations
void ANpcBase::Initialize_Implementation(APool* PoolValue)
{
	IPoolable::Initialize_Implementation(PoolValue);
	this->Pool = PoolValue;
	SetActorHiddenInGame(true);

	MovCompPtr->SetComponentTickEnabled(false);

	if (IsValid(GetCurrentWeapon()) && startWithWeapon)
	{
		GetCurrentWeapon()->SetActorHiddenInGame(true);
	}

	for (const auto& [Channel, Response] : InitCollisionChannelResponses)
	{
		CapsulePtr->SetCollisionResponseToChannel(Channel, Response);
	}

	MovCompPtr->StopMovementImmediately();
	MeshPtr->SetComponentTickEnabled(false);
	integrityComponent->AddImmortalitySource();
	ToggleAI(false);
}

// Pool implementations
void ANpcBase::OnObjectPooled_Implementation()
{
	IPoolable::OnObjectPooled_Implementation();

	MovCompPtr->SetComponentTickEnabled(true);

	NpcSubsystem->OnEnemySpawned.Broadcast(this);

	bIsPooled = true;
	integrityComponent->RemoveImmortalitySource();
	UE_LOG(LogPool, Display, TEXT("[NPCBase %s] is required from pool!"), *this->GetName())
	SetActorHiddenInGame(false);
	MeshPtr->SetComponentTickEnabled(true);

	MeshPtr->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f), false, nullptr, ETeleportType::ResetPhysics);
	MeshPtr->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.0f, 0.0f, -90.0f)));

	if (IsValid(GetCurrentWeapon()) && startWithWeapon)
	{
		GetCurrentWeapon()->SetActorHiddenInGame(false);
	}

	FTimerHandle OnPooledTimer;

	// HACK Try to fix Level Design flaws for enemy spawn positions
	GetWorldTimerManager().SetTimer(OnPooledTimer, this, &ANpcBase::ToggleCapsuleCollision, .15f);

	ToggleAI(true);
}

// Pool implementations
void ANpcBase::OnObjectReturnToPool_Implementation()
{
	IPoolable::OnObjectReturnToPool_Implementation();
	UE_LOG(LogPool, Display, TEXT("[NPCBase %s] returning to pool!"), *this->GetName())
	SetActorHiddenInGame(true);
	bIsPooled = false;
	integrityComponent->FullRestore();
	bIsAlive = true;

	integrityComponent->AddImmortalitySource();

	if (IsValid(GetCurrentWeapon()) && startWithWeapon)
	{
		GetCurrentWeapon()->SetActorHiddenInGame(true);
	}

	UnsetRagdoll();

	MovCompPtr->StopMovementImmediately();

	CapsulePtr->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Ignore);
	for (const auto& [Channel, Response] : InactiveCollisionChannelResponses)
	{
		CapsulePtr->SetCollisionResponseToChannel(Channel, Response);
	}

	MeshPtr->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f), false, nullptr, ETeleportType::ResetPhysics);
	MeshPtr->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.0f, 0.0f, -90.0f)));

	MovCompPtr->SetComponentTickEnabled(false);
	MeshPtr->SetComponentTickEnabled(false);
	ToggleAI(false);
}

bool ANpcBase::TryCacheVfxPool()
{
	// Look for cached pool
	if (!VfxPool)
	{
		// Try re-caching pool
		VfxPool = NpcSubsystem->GetVfxPool();

		if (!VfxPool)
		{
			UE_LOG(LogPool, Error, TEXT("Missing VFX pool"));
			return false;
		}
	}

	return true;
}

void ANpcBase::ResetNpcState()
{
	MovCompPtr->SetMovementMode(MOVE_Walking);

	// Physics
	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	for (const auto& [Channel, Response] : InactiveCollisionChannelResponses)
	{
		CapsulePtr->SetCollisionResponseToChannel(Channel, Response);
	}

	// Integrity
	bIsAlive = true;
	integrityComponent->FullRestore();
	integrityComponent->RemoveImmortalitySource();
}

void ANpcBase::DropLoot()
{
	if (LootTable.IsEmpty()) return;

	if (UWorld* World = GetWorld())
	{
		const FVector DefaultSpawnLocation = GetActorLocation();

		for (int32 i = 0; i < LootTable.Num(); i++)
		{
			UClass* DropClass = LootTable[i].Loot.Get();
			if (!DropClass)
			{
				continue;
			}

			if (NpcSubsystem->GetLootDropExclusionList().Contains(DropClass))
			{
				continue;
			}

			if (FMath::RandRange(0, 100) <= LootTable[i].DropProbability)
			{
				World->SpawnActor<AActor>(LootTable[i].Loot, DefaultSpawnLocation + LootTable[i].DropOffset,
				                          FRotator::ZeroRotator);
			}
		}
	}
}

void ANpcBase::OnEnemyHasToStop()
{
	AutoReturnToPool();
}
