#include "NPCs/Characters/KamikazeNpc.h"

#include "NiagaraSystem.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NPCs/Controllers/KamikazeNpcController.h"
#include "NPCs/Data/KamikazeEnemyData.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "VFXSystem/Actors/EnemiesVfxMultiPool.h"

void AKamikazeNpc::SetupFromConfig()
{
	Super::SetupFromConfig();

	if (const UKamikazeEnemyData* Data = Cast<UKamikazeEnemyData>(EnemyData))
	{
		MovementAcceleration = Data->AccelerationSpeed * 100.f;
		MovCompPtr->MaxAcceleration = MovementAcceleration;
		TurnAcceleration = Data->TurnAcceleration;
		MaxTurnRate = Data->MaxTurnRate;
		MovCompPtr->RotationRate = FRotator(0.0f, 0.0f, MaxTurnRate);
		MinFacingTolerance = Data->MinFacingTolerance;
		ExplosionRadius = Data->HitSphereRadius;
		ExplosionDamage = Data->AttackDamage;
		ExplosionObjDamageQuery = Data->ExplosionObjDamageQuery;

		FuzeDetonationTime = Data->FuzeDetonationTime;

		KamikazeController = Cast<AKamikazeNpcController>(GetController<AKamikazeNpcController>());

		if (KamikazeController)
		{
			KamikazeController->SetWindupTime(TimeBetweenAttacks);
		}

		TArray<FSoftObjectPath> SfxPaths;

		if (!Data->ExplosionSfx.IsNull())
		{
			SfxPaths.Add(Data->ExplosionSfx.ToSoftObjectPath());
		}

		if (!Data->AttackAnimMontage.IsNull())
		{
			SfxPaths.Add(Data->AttackAnimMontage.ToSoftObjectPath());
		}

		if (SfxPaths.Num() < 0) return;

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		Streamable.RequestAsyncLoad(
			SfxPaths, FStreamableDelegate::CreateUObject(this, &AKamikazeNpc::OnCosmeticsLoaded));
	}
}

void AKamikazeNpc::OnCosmeticsLoaded()
{
	Super::OnCosmeticsLoaded();
	const UKamikazeEnemyData* Data = CastChecked<UKamikazeEnemyData>(EnemyData);
	ExplosionSound = Data->ExplosionSfx.Get();
	AttackAnimMontage = Data->AttackAnimMontage.Get();
}

void AKamikazeNpc::Attack(AActor* Target)
{
	if (!bIsAlive)
	{
		return;
	}

	const bool bIsDetonating = KamikazeController->GetIsDetonating();

	if (FuzeDetonationTime <= 0.0f)
	{
		Explode(this);
		return;
	}

	if (!bIsDetonating)
	{
		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AKamikazeNpc::OnDetonationTimerEnded, FuzeDetonationTime);
		KamikazeController->SetIsDetonating(true);
	}
}

void AKamikazeNpc::OnDetonationTimerEnded()
{
	Explode(this);
}

void AKamikazeNpc::CancelDetonation()
{
	if (GetWorldTimerManager().IsTimerActive(ExplosionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
		KamikazeController->SetIsDetonating(false);
	}
}

void AKamikazeNpc::Explode(const AActor* Killer)
{
	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);

	bIsAlive = false;

	CancelDetonation();

	bool bHasHitPlayer = MakeAOEDamage(ExplosionRadius, ExplosionDamage, ExplosionObjDamageQuery, false,
	                                   ExplosionSound.Get(),
	                                   GetDeathVfxPtr());

	OnPlayerHitByExplosionDelegate.Broadcast(bHasHitPlayer);

	ToggleAI(false);

	DropLoot();

	NpcSubsystem->OnEnemyDied.Broadcast(this, PointsPerKill, Killer);

	AutoReturnToPool();

	ResetNpcState();
}

void AKamikazeNpc::ResetNpcState()
{
	KamikazeController->SetIsDetonating(false);
	Super::ResetNpcState();
}

void AKamikazeNpc::HandleCharacterDeath(const AActor* character, const AActor* harmer,
                                        const class UIntegrity* component, const float currentIntegrity)
{
	if (!bIsAlive) return;
	
	Explode(harmer);
}
