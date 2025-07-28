#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "RotateTowardsTask.generated.h"

UCLASS()
class TBO02_API URotateTowardsTask : public UBTTaskNode
{
	GENERATED_BODY()
	//Fields
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate Towards")
	FBlackboardKeySelector RotationTarget;

	URotateTowardsTask();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner, uint8* nodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& owner, uint8* nodeMemory, float deltaSeconds) override;

private:
	void ComputeRotation(const UBehaviorTreeComponent& Owner, const float DeltaTime);
};
