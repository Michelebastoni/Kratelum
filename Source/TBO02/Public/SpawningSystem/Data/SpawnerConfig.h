#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpawnerConfig.generated.h"

class UPoolData;
struct FEnemiesToKillPerPhase;

/**
 * Data asset containing necessary information of an enemy spawner.\n
 * <see cref="https://docs.google.com/document/d/1USdblgByXY4VRJT549L-f0F3BKO41jOH7PX9TwMWscU/edit?tab=t.pmtyjgy3pg56"/>
 */
UCLASS(BlueprintType)
class TBO02_API USpawnerConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Minimum number of new groups of enemies.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Minimum number of new groups of enemies."))
	int32 MinEnemyGroupNumber;

	/**
	 * The total number of enemies to spawn before spawner auto shuts down.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The total number of enemies to spawn before spawner auto shuts down."))
	int32 TotalEnemiesToSpawn;

	/**
	 * Initial spawn probability for a new group of enemies.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Initial spawn probability for a new group of enemies."))
	float SpawnProb;

	/**
	 * The amount of spawn probability to increase for each iteration.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The amount of spawn probability to increase for each iteration."))
	float SpawnPercentDelta;

	/**
	 * Number of enemies to reach before having a 100% spawn rate.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Number of enemies to reach before having 100% spawn rate."))
	int32 EnemyGroupCap;

	/**
	 * How many seconds to wait before triggering the timer for the new phase. 
	 */
	UPROPERTY(EditAnywhere,
		meta = (ToolTip = "How many seconds to wait before triggering the timer for the new phase."))
	float DelayBetweenPhases;

	UPROPERTY(EditAnywhere,
		meta = (ToolTip = "How much should the spawn location of a single enemy be randomized? (X)"))
	float XAxisSpawnDeviation;

	UPROPERTY(EditAnywhere,
		meta = (ToolTip = "How much should the spawn location of a single enemy be randomized? (Y)"))
	float YAxisSpawnDeviation;

	/**
	 * Pairs of phase-enemies to kill to trigger it.
	 */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Pairs of phase-enemies to kill to trigger it."))
	TArray<FEnemiesToKillPerPhase> Phases;
};
