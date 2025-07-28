#pragma once

#include "CoreMinimal.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "RangedEnemyData.generated.h"

UCLASS()
class TBO02_API URangedEnemyData : public UBaseEnemyData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings")
	float MinDeviationAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings")
	float MaxDeviationAngle;
};
