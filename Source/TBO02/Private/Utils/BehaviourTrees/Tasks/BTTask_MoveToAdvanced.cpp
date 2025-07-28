#include "Utils/BehaviourTrees/Tasks/BTTask_MoveToAdvanced.h"

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask_MoveTo.h"

EBTNodeResult::Type UBTTask_MoveToAdvanced::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::PerformMoveTask(OwnerComp, NodeMemory);
	
	return EBTNodeResult::InProgress;
}
