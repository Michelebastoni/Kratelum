#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ComputeNewMovementTarget.generated.h"

/**
 * Task used by Laser Caller NPC Controller to trigger the farthest point logic.
 */
UCLASS()
class TBO02_API UBTTask_ComputeNewMovementTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
