#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsInRange.generated.h"

/**
 * Checks if the blackboard's bool value representing the in-range value is true or not.
 */
UCLASS()
class TBO02_API UBTDecorator_IsInRange : public UBTDecorator_Blackboard
{
	GENERATED_BODY()
	float LoS = 20.0f;
	
public:
	UBTDecorator_IsInRange();
	
protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
