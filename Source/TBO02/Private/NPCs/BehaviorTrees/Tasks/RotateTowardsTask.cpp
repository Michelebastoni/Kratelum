#include "NPCs/BehaviorTrees/Tasks/RotateTowardsTask.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Controllers/NpcControllerBase.h"

URotateTowardsTask::URotateTowardsTask()
{
	NodeName = "Rotate Towards";

	bNotifyTick = true;

	RotationTarget.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, RotationTarget),
	                                  AActor::StaticClass());
}

EBTNodeResult::Type URotateTowardsTask::ExecuteTask(UBehaviorTreeComponent& owner, uint8* nodeMemory)
{
	return EBTNodeResult::InProgress;
}

void URotateTowardsTask::TickTask(UBehaviorTreeComponent& owner, uint8* nodeMemory, float deltaSeconds)
{
	ComputeRotation(owner, deltaSeconds);

	FinishLatentTask(owner, EBTNodeResult::Succeeded);
}

void URotateTowardsTask::ComputeRotation(const UBehaviorTreeComponent& Owner, const float DeltaTime)
{
	if (const ANpcControllerBase* NpcController = Cast<ANpcControllerBase>(Owner.GetAIOwner()))
	{
		if (ANpcBase* NPC = Cast<ANpcBase>(NpcController->GetPawn()))
		{
			AActor* Target = Cast<AActor>(
				Owner.GetBlackboardComponent()->GetValueAsObject(RotationTarget.SelectedKeyName));
			NPC->UpdateRotation(DeltaTime, Target);
		}
	}
}
