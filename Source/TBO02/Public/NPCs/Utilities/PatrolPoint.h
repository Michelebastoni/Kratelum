#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "PatrolPoint.generated.h"

UCLASS()
class TBO02_API APatrolPoint : public ATargetPoint
{
	GENERATED_BODY()
protected:
	APatrolPoint();
	virtual void BeginPlay() override;
};
