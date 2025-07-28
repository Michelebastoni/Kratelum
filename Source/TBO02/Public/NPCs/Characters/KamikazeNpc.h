#pragma once

#include "CoreMinimal.h"
#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Controllers/KamikazeNpcController.h"
#include "KamikazeNpc.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitDelegate, bool, bHasHitPlayer);

UCLASS(Abstract)
class TBO02_API AKamikazeNpc : public ANpcBase
{
	GENERATED_BODY()

	// Runtime data cache
	float MovementAcceleration;
	float TurnAcceleration;
	float MaxTurnRate;
	float MinFacingTolerance;

	float ExplosionRadius;
	float ExplosionDamage;
	float FuzeDetonationTime;
	FTimerHandle ExplosionTimerHandle;

	UPROPERTY()
	TObjectPtr<UAnimMontage> AttackAnimMontage;

	UPROPERTY()
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Settings")
	TArray<TEnumAsByte<EObjectTypeQuery>> ExplosionObjDamageQuery;

	UPROPERTY(Transient)
	FVector LastTargetKnowPosition;

	UPROPERTY(Transient)
	AKamikazeNpcController* KamikazeController;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerHitDelegate OnPlayerHitByExplosionDelegate;

	virtual void SetupFromConfig() override;
	virtual void OnCosmeticsLoaded() override;

	void Explode(const AActor* Killer);
	
	UFUNCTION()
	void OnDetonationTimerEnded();
	virtual void Attack(AActor* Target) override;
	void CancelDetonation();
	virtual void ResetNpcState() override;
	virtual void HandleCharacterDeath(const AActor* character, const AActor* harmer, const class UIntegrity* component,
	                                  const float currentIntegrity) override;
};
