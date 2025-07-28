#include "NPCs/Controllers/MeleeNpcController.h"

#include "BehaviorTree/BlackboardComponent.h"

void AMeleeNpcController::SetWindupTime(const float Value)
{
	Blackboard->SetValueAsFloat(AttackWindupTimeKey, Value);
}
