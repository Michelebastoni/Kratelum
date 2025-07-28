#include "NPCs/Characters/MeleeNpc.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NPCs/Controllers/MeleeNpcController.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "NPCs/Data/MeleeEnemyData.h"
#include "NPCs/Utilities/DamageSphere.h"
#include "Weapons/MeleeComponent.h"

AMeleeNpc::AMeleeNpc()
{
	DamageSphere = CreateDefaultSubobject<UDamageSphere>("DamageSphere");
	DamageSphere->SetupAttachment(RootComponent);
}

void AMeleeNpc::SetupFromConfig()
{
	Super::SetupFromConfig();

	if (const UMeleeEnemyData* Data = Cast<UMeleeEnemyData>(EnemyData))
	{
		MovementAcceleration = Data->AccelerationSpeed * 100.f;
		GetCharacterMovement()->MaxAcceleration = MovementAcceleration;
		TurnAcceleration = Data->TurnAcceleration;
		MaxTurnRate = Data->MaxTurnRate;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, MaxTurnRate);
		MinFacingTolerance = Data->MinFacingTolerance;
		HitSphereRadius = Data->HitSphereRadius;
		AttackDamage = Data->AttackDamage;

		if (DamageSphere)
		{
			DamageSphere->Init(HitSphereRadius, AttackDamage);
		}

		if (AMeleeNpcController* Ctrl = Cast<AMeleeNpcController>(GetController<AMeleeNpcController>()))
		{
			Ctrl->SetWindupTime(TimeBetweenAttacks);
		}

		TArray<FSoftObjectPath> CosmeticEffectsPaths;

		if (!Data->AttackAnimMontage.IsNull())
		{
			CosmeticEffectsPaths.Add(Data->AttackAnimMontage.ToSoftObjectPath());
		}

		if (CosmeticEffectsPaths.Num() > 0)
		{
			FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

			StreamManager.RequestAsyncLoad(CosmeticEffectsPaths,
			                               FStreamableDelegate::CreateUObject(
				                               this, &AMeleeNpc::OnCosmeticsLoaded));
		}
	}
}

void AMeleeNpc::Attack(AActor* Target)
{
	PlayAnimMontage(AttackAnimMontage.Get());
}

void AMeleeNpc::ToggleDamageSphere(const bool Value) const
{
	DamageSphere->Toggle(Value);
}

void AMeleeNpc::ResetNpcState()
{
	Super::ResetNpcState();
}

void AMeleeNpc::OnCosmeticsLoaded()
{
	Super::OnCosmeticsLoaded();
	
	AttackAnimMontage = CastChecked<UMeleeEnemyData>(EnemyData)->AttackAnimMontage.Get();
}
