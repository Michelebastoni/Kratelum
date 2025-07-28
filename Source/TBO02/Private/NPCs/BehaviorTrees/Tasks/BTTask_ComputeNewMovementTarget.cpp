#include "NPCs/BehaviorTrees/Tasks/BTTask_ComputeNewMovementTarget.h"

#include "NPCs/Controllers/LaserCallerNpcController.h"

EBTNodeResult::Type UBTTask_ComputeNewMovementTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ALaserCallerNpcController* LaserNpcController = Cast<ALaserCallerNpcController>(OwnerComp.GetAIOwner());
	if (!LaserNpcController)
	{
		return EBTNodeResult::Failed;
	}

	LaserNpcController->ComputeNewMovementTarget();
	return EBTNodeResult::Succeeded;
}
