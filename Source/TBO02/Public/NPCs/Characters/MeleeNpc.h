#pragma once

#include "CoreMinimal.h"
#include "NPCs/Characters/NpcBase.h"
#include "MeleeNpc.generated.h"

class UDamageSphere;

UCLASS(Abstract)
class TBO02_API AMeleeNpc : public ANpcBase
{
	GENERATED_BODY()

protected:
	// Runtime data cache
	float MovementAcceleration;
	float TurnAcceleration;
	float MaxTurnRate;
	float MinFacingTolerance;

	float HitSphereRadius;
	float AttackDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDamageSphere> DamageSphere;

	UPROPERTY()
	TObjectPtr<UAnimMontage> AttackAnimMontage;
	
public:
	AMeleeNpc();

	virtual void SetupFromConfig() override;
	virtual void OnCosmeticsLoaded() override;
	
	virtual void Attack(AActor* Target) override;
	void ToggleDamageSphere(bool Value) const;
	virtual void ResetNpcState() override;
};
