#pragma once

#include "CoreMinimal.h"
#include "NPCs/Characters/NpcBase.h"
#include "LaserCallerNpc.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyPooledDelegate, bool, bIsActive);

class ATrap;

UCLASS(Abstract)
class TBO02_API ALaserCallerNpc : public ANpcBase
{
	GENERATED_BODY()

protected:
	float TimeToWaitBeforeCall;
	float TimeToWaitAfterCall;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CallLaserAnimMontage;

	UPROPERTY(Transient)
	TObjectPtr<ATrap> OrbitalLaser;

public:
	UPROPERTY(BlueprintAssignable)
	FOnEnemyPooledDelegate OnEnemyPooledDelegate;

	float GetTimeToWaitBeforeCall() const { return TimeToWaitBeforeCall; }
	float GetTimeToWaitAfterCall() const { return TimeToWaitAfterCall; }

	virtual void SetupFromConfig() override;
	virtual void OnCosmeticsLoaded() override;

	virtual void Attack(AActor* Target) override;
	virtual void HandleCharacterDeath(const AActor* character, const AActor* harmer, const class UIntegrity* component,
	                                  const float currentIntegrity) override;

	void CallLaser();

	virtual void OnObjectPooled_Implementation() override;
	virtual void OnObjectReturnToPool_Implementation() override;
};
