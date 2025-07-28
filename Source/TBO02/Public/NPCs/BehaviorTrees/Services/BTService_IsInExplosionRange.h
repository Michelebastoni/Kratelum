#pragma once

#include "CoreMinimal.h"
#include "NPCs/BehaviorTrees/Services/BTService_IsInRange.h"
#include "BTService_IsInExplosionRange.generated.h"

UCLASS()
class TBO02_API UBTService_IsInExplosionRange : public UBTService_IsInRange
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
