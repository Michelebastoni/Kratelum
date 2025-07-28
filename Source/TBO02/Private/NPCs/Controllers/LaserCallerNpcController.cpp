#include "NPCs/Controllers/LaserCallerNpcController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "NPCs/Characters/LaserCallerNpc.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "NPCs/Utilities/PatrolPoint.h"

void ALaserCallerNpcController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const UWorld* World = GetWorld())
	{
		PatrolPoints = UNPCSubsystem::GetInstance(World)->GetPatrolPoints();

		if (const ALaserCallerNpc* LaserCaller = Cast<ALaserCallerNpc>(GetPawn()))
		{
			Blackboard->SetValueAsObject(PatrolPointKey, GetPawn());
			Blackboard->SetValueAsFloat(TimeToWaitBeforeCallKey, LaserCaller->GetTimeToWaitBeforeCall());
			Blackboard->SetValueAsFloat(TimeToWaitAfterCallKey, LaserCaller->GetTimeToWaitAfterCall());
		}

		if (Player && PatrolPoints.Num() > 0)
		{
			ComputeNewMovementTarget();
			UpdateBlackboardMovementTarget();
		}
	}
}

void ALaserCallerNpcController::SetPlayerTarget(APlayerCharacter* Target)
{
	Player = Target;

	Blackboard->SetValueAsObject(TargetKey, GetPawn());
	if (const UWorld* World = GetWorld())
	{
		PatrolPoints = UNPCSubsystem::GetInstance(World)->GetPatrolPoints();

		if (PatrolPoints.Num() > 0)
		{
			ComputeNewMovementTarget();
			UpdateBlackboardMovementTarget();
		}
	}
}

void ALaserCallerNpcController::ComputeNewMovementTarget()
{
	if (!Player || PatrolPoints.Num() == 0) return;

	int FurthestPointIndex = -1;
	const FVector PlayerPos = Player->GetActorLocation();
	float FurthestPointSqrtDistance = -1.0f;

	for (int i = 0; i < PatrolPoints.Num(); i++)
	{
		if ((CurrentTargetIndex != -1 && i == CurrentTargetIndex) ||
			(PreviousTargetIndex != -1 && i == PreviousTargetIndex))
		{
			continue;
		}

		const float CurrentSqrtDist = FVector::DistSquaredXY(PlayerPos, PatrolPoints[i]->GetActorLocation());

		if (CurrentSqrtDist > FurthestPointSqrtDistance)
		{
			FurthestPointIndex = i;
			FurthestPointSqrtDistance = CurrentSqrtDist;
		}
	}

	if (FurthestPointIndex == -1 && CurrentTargetIndex != -1)
	{
		FurthestPointIndex = CurrentTargetIndex;
	}
	else if (FurthestPointIndex == -1)
	{
		FurthestPointIndex = 0;
	}

	PreviousTargetIndex = CurrentTargetIndex;
	CurrentTargetIndex = FurthestPointIndex;

	CurrentMovementTarget = PatrolPoints[FurthestPointIndex];

	UpdateBlackboardMovementTarget();
}

void ALaserCallerNpcController::UpdateBlackboardMovementTarget() const
{
	if (!CurrentMovementTarget)
	{
		Blackboard->SetValueAsObject(PatrolPointKey, nullptr);
	}
	else
	{
		Blackboard->SetValueAsObject(PatrolPointKey, CurrentMovementTarget.Get());
	}
}
