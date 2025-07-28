#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_IsInRange.generated.h"

class ANpcBase;

/**
 * Performs distance checks to see if the enemy's target is in range
 * and updates the corresponding blackboard's bool value.
 */
UCLASS()
class TBO02_API UBTService_IsInRange : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_IsInRange();
	
protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsInRangeKey;

	UPROPERTY(EditAnywhere)
	bool bShouldConsiderOrientation = true;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
