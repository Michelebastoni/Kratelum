#pragma once

#include "CoreMinimal.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "MeleeEnemyData.generated.h"

UCLASS()
class TBO02_API UMeleeEnemyData : public UBaseEnemyData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings", meta = (Units = "cm"))
	float HitSphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings")
	float AttackDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings")
	TSoftObjectPtr<UAnimMontage> AttackAnimMontage;
};
