#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerableEnemySpawner.generated.h"

class ASpawnLocation;
class ANpcBase;
class APool;

UCLASS()
class TBO02_API ATriggerableEnemySpawner : public AActor
{
	GENERATED_BODY()

protected:
	/**
	 * TMap representing which enemy type should be spawned and in which amount.\n
	 * (i.e., groups of same-type enemies)
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TMap<TSubclassOf<ANpcBase>, int32> EnemiesGroups;

	/**
	 * The pools from which enemies will be taken.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TArray<TObjectPtr<APool>> EnemyPools;

	/**
	 * How far can the enemy spawn from the spawn location center on the X axis 
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float XAxisRandomSpawnDeviation;

	/**
	 * How far can the enemy spawn from the spawn location center on the Y axis 
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float YAxisRandomSpawnDeviation;

	/**
	 * Should this spawner automatically trigger the spawn action
	 * when trigger kill counter reaches the threshold?
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bAutoTriggerAtThreshold;

	/**
	 * How many enemies should be killed to auto-trigger the spawn action.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,
		meta = (EditCondition = bAutoTriggerAtThreshold, ClampMin = "1", UIMin = "1"))
	int32 TriggerKillThreshold;

	/**
	 * Where to spawn the enemy or the groups of enemies.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (EditCondition = bAutoTriggerAtThreshold))
	TObjectPtr<ASpawnLocation> DefaultSpawnLocation;

	/**
	 * Should the spawner spawn a single enemy?
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (EditCondition = bAutoTriggerAtThreshold))
	bool bSpawnSingle = true;

	/**
	 * Which single enemy to spawn? (Must be in the pools to be picked)
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (EditCondition = bSpawnSingle))
	TSubclassOf<ANpcBase> EnemyToSpawn;

	/**
	 * Should the kill counter reset after the threshold is reached?\n
	 * If set to <b>false</b>, the spawner behaves like a <i>DO ONCE</i> node.
	 * If set to <b>true</b>, the spawn action can be reiterated.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (EditCondition = bAutoTriggerAtThreshold))
	bool bResetKillCounterAfterTrigger;

	UPROPERTY(Transient)
	TMap<TSubclassOf<AActor>, TObjectPtr<APool>> EnemiesGroupsMap;

	UPROPERTY(Transient)
	int32 KilledEnemies;
	
	UPROPERTY(Transient)
	bool bCanSpawn = true;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ATriggerableEnemySpawner();

	/**
	 * Spawns all groups from the EnemiesGroups map at the given location (applying random offset).
	 * @param SpawnLocation Where to spawn the group.
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnGroups(const FTransform& SpawnLocation);

	/**
	 * Spawns a specific NPC if contained in the groups' map at the given location (applying random offset)
	 * @param NpcClass The NPC to spawn.
	 * @param SpawnLocation  Where to spawn the NPC.
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnSingle(TSubclassOf<ANpcBase> NpcClass, const FTransform& SpawnLocation);

	/**
	 * Callback triggered whenever an enemy dies.\n
	 * If auto-spawn logic is enabled, it will trigger the spawner
	 * as soon as the kill threshold is reached.
	 * @param KilledEnemy The enemy that died.
	 * @param PointsPerKill How many points that enemy is worth.
	 * @param Killer Who killed the enemy.
	 */
	UFUNCTION()
	void OnEnemyDied(const AActor* KilledEnemy, const int32 PointsPerKill, const AActor* Killer);

private:
	/**
	 * Method to apply random deviation to the given spawn location.
	 * @param SpawnLocation Starting point.
	 * @return The randomized spawn location.
	 */
	FVector GetRandomSpawnLocation(const FTransform& SpawnLocation) const;

	UFUNCTION()
	void OnStopSpawner();
	
protected:
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override
	{
		const bool ParentVal = Super::CanEditChange(InProperty);
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, EnemyToSpawn))
		{
			// Ensure EnemyToSpawn field is editable
			// only if AutoTrigger and SpawnSingle are toggled.
			return ParentVal && bAutoTriggerAtThreshold && bSpawnSingle;
		}
		return ParentVal;
	}
#endif
};
