#include "NPCs/BehaviorTrees/Services/BTService_IsInExplosionRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NPCs/Characters/KamikazeNpc.h"

void UBTService_IsInExplosionRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;
	
	const AKamikazeNpcController* Controller = Cast<AKamikazeNpcController>(OwnerComp.GetAIOwner());
	if (!Controller) return;
	
	AKamikazeNpc* Kamikaze = Cast<AKamikazeNpc>(Controller->GetPawn());
	if (!Kamikaze) return;

	if (!Blackboard->GetValueAsBool(IsInRangeKey.SelectedKeyName) && Controller->GetIsDetonating())
	{
		Kamikaze->CancelDetonation();
	}
}
