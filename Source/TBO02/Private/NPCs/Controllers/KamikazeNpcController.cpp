#include "NPCs/Controllers/KamikazeNpcController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

void AKamikazeNpcController::BeginPlay()
{
	Super::BeginPlay();
	
	GetPathFollowingComponent()->SetStopMovementOnFinish(false);
}

void AKamikazeNpcController::SetWindupTime(const float Value)
{
	Blackboard->SetValueAsFloat(AttackWindupTimeKey, Value);
}

void AKamikazeNpcController::SetIsDetonating(const bool bValue) const
{
	Blackboard->SetValueAsBool(IsDetonatingKey, bValue);
	OnDetonationValueChangedDelegate.Broadcast(bValue);
}

bool AKamikazeNpcController::GetIsDetonating() const
{
	return Blackboard->GetValueAsBool(IsDetonatingKey);
}
