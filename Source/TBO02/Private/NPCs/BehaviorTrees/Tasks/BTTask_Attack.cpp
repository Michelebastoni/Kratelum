#include "NPCs/BehaviorTrees/Tasks/BTTask_Attack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NPCs/Characters/NpcBase.h"


UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Attack Target";

	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Attack, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	ANpcBase* NPC = Cast<ANpcBase>(AIPawn);
	if (!NPC) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;
	
	UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return EBTNodeResult::Failed;

	NPC->Attack(TargetActor);

	return EBTNodeResult::Succeeded;
}