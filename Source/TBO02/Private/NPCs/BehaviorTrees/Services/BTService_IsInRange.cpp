#include "NPCs/BehaviorTrees/Services/BTService_IsInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPCs/Characters/NpcBase.h"

UBTService_IsInRange::UBTService_IsInRange()
{
	NodeName = "Check if Target is in Range";
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsInRange, TargetKey), AActor::StaticClass());
	IsInRangeKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsInRange, IsInRangeKey));
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_IsInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	const ANpcBase* Enemy = Cast<ANpcBase>(AIPawn);
	if (!Enemy) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	const AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target) return;

	// Distance check, uses squared distances to save some performance
	const float AttackRangeSqr = Enemy->GetSqrAttackRange();
	const float CurrentDistanceSqr = FVector::DistSquaredXY(Target->GetActorLocation(), Enemy->GetActorLocation());

	bool bShouldAttack = CurrentDistanceSqr < AttackRangeSqr;
	
	if (bShouldConsiderOrientation)
	{
		bool bIsInView = false;
		const float FacingTolerance = Enemy->GetFacingTolerance();
		const float TargetYaw = (Target->GetActorLocation() - Enemy->GetActorLocation()).
		                        GetSafeNormal2D().Rotation().Yaw;
		const float CurrentYaw = Enemy->GetActorRotation().Yaw;

		const float YawDelta = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
		
		bShouldAttack &= FMath::Abs(YawDelta) < FacingTolerance;
	}

	// Updates blackboard value
	Blackboard->SetValueAsBool(IsInRangeKey.SelectedKeyName, bShouldAttack);
}
