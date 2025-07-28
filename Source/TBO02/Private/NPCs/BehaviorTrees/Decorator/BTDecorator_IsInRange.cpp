#include "NPCs/BehaviorTrees/Decorator/BTDecorator_IsInRange.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInRange::UBTDecorator_IsInRange()
{
	NodeName = "Is Target in range?";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

bool UBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	return Blackboard && Blackboard->GetValueAsBool(GetSelectedBlackboardKey());
}
