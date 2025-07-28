#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyPhaseData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyWeightEntry
{
	GENERATED_BODY()

	/**
	* The enemy to spawn
	*/
	UPROPERTY(EditAnywhere, meta = (MustImplement = "/Script/TBO02.Poolable", AllowAbstract = false))
	TSubclassOf<AActor> EnemyClass;

	/**
	 * The weight distribution probability [0-100]
	 */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int Weight = 0;
};

/**
 * <see cref="https://docs.google.com/document/d/1USdblgByXY4VRJT549L-f0F3BKO41jOH7PX9TwMWscU/edit?tab=t.pmtyjgy3pg56#heading=h.qhch273rsdzv"/>
 */
UCLASS(BlueprintType)
class TBO02_API UEnemyPhaseData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * The total number of enemies that can be concurrently alive.
	 */
	UPROPERTY(EditAnywhere, meta = (Tooltip = "The total amount of enemies that can be concurrently alive."))
	int MaxEnemiesInArena;

	/**
	 * Delay between each enemy (of a group) spawn.
	 */
	UPROPERTY(EditAnywhere, meta = (Tooltip = "Delay between each enemy (of a group) spawn."))
	float Delay;

	/**
	 * Weight for each enemy to spawn.
	 */
	UPROPERTY(EditAnywhere, meta = (Tooltip = "Weight for each enemy to spawn."))
	TArray<FEnemyWeightEntry> EnemyWeights;
};

USTRUCT(BlueprintType)
struct FEnemiesToKillPerPhase
{
	GENERATED_BODY()

	/**
	 * The tot number of enemies to kill to activate this phase.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The tot number of enemies to kill to activate this phase."))
	int EnemiesToKill = 0;

	/**
	 * The phase that will be activated.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The phase that will be activated."))
	TObjectPtr<UEnemyPhaseData> Phase;
};
