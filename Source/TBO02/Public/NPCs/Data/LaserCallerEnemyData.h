#pragma once

#include "CoreMinimal.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "LaserCallerEnemyData.generated.h"

class ATrap;
class UAnimMontage;

UCLASS()
class TBO02_API ULaserCallerEnemyData : public UBaseEnemyData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Parameters|Combat Settings")
	float TimeToWaitBeforeLaserCall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Parameters|Combat Settings")
	float TimeToWaitAfterLaserCall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Parameters|Combat Settings")
	TSubclassOf<ATrap> OrbitalLaser;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Parameters|Cosmetic")
	TSoftObjectPtr<UAnimMontage> CallLaserAnimMontage;
};
