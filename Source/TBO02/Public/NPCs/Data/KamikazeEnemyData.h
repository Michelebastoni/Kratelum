#pragma once

#include "CoreMinimal.h"
#include "MeleeEnemyData.h"
#include "KamikazeEnemyData.generated.h"

UCLASS()
class TBO02_API UKamikazeEnemyData : public UMeleeEnemyData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Combat Settings")
	TArray<TEnumAsByte<EObjectTypeQuery>> ExplosionObjDamageQuery;

	//, meta = (ClampMin = "0.1", UIMin = "0.1")
	UPROPERTY(EditDefaultsOnly, Category = "Combat Settings")
	float FuzeDetonationTime = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Settings|Cosmetic")
	TSoftObjectPtr<USoundBase> ExplosionSfx;
};
