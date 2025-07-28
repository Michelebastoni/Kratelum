#pragma once

#include "CoreMinimal.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "LaserCallerNpcController.generated.h"

class APatrolPoint;

UCLASS()
class TBO02_API ALaserCallerNpcController : public ANpcControllerBase
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<APatrolPoint>> PatrolPoints;

	UPROPERTY(Transient)
	TObjectPtr<APatrolPoint> CurrentMovementTarget;
	
	int32 CurrentTargetIndex = -1;
	int32 PreviousTargetIndex = -1;

protected:
	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName PatrolPointKey = "PatrolPoint";
	
	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName TimeToWaitBeforeCallKey = "BeforeAttackTime";
	
	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName TimeToWaitAfterCallKey = "AfterAttackTime";

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetPlayerTarget(APlayerCharacter* Target) override;

	/**
	 * Get the furthest patrol point away from the player.
	 */
	void ComputeNewMovementTarget();

private:
	void UpdateBlackboardMovementTarget() const;
};
