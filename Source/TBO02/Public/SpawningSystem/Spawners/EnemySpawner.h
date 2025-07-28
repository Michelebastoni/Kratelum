#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class UEnemyPhaseData;
class ASpawnLocation;
class USpawnerConfig;
class UPoolData;
class APool;

DECLARE_MULTICAST_DELEGATE_FourParams(FOnPhaseCompleted, const UEnemyPhaseData*, const UEnemyPhaseData*, const int, const bool);

/**
 * Runtime-only struct to monitor and cache a spawner's state.
 */
USTRUCT()
struct FSpawnerState
{
	GENERATED_BODY()

	UPROPERTY()
	UEnemyPhaseData* CurrentPhase = nullptr;

	int32 AliveEnemies = 0;
	int32 PhaseEnemiesKilled = 0;
	int32 CurrentGroupSize = 0;
	int32 CurrentPhaseIndex = 0;
	int32 TotalSpawnedEnemies = 0;
	int32 LastSpawnPointIndex = 0;
	float CurrentSpawnProb = 0.0f;
	bool bCanSpawn = false;
};

UCLASS(BlueprintType)
class TBO02_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

	// Spawning cache
	int32 RemainingEnemiesToSpawn = 0;
	FVector CachedSpawnPoint;
	
public:
	/**
	 * Static delegate notifying that the spawner has completed a phase.\n
	 * Passes the new phase index and the new phase data as parameters.
	 */
	static FOnPhaseCompleted OnPhaseCompleted;

	AEnemySpawner();

	UPROPERTY(EditAnywhere)
	bool bDrawDebugInfo;

protected:
	/**
	 * Config file with essential spawning params and phases config refs.
	 */
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<USpawnerConfig> SpawnerConfig;

	UPROPERTY(EditInstanceOnly)
	bool bAutoStart = true;

	/**
	 * How many seconds to wait before triggering spawn logic.
	 */
	UPROPERTY(EditInstanceOnly)
	float InitialSpawnDelay;

	/**
	 * The spawn points that will be used to position the enemies.
	 */
	UPROPERTY(EditInstanceOnly)
	TArray<ASpawnLocation*> SpawnLocations;

	UPROPERTY(EditInstanceOnly)
	TArray<APool*> Pools;

	/**
	 * Fast accessing cache for using pools efficiently.
	 */
	UPROPERTY(Transient)
	TMap<TSubclassOf<AActor>, APool*> PoolMap;

	/**
	 * Runtime-only struct to monitor and cache a spawner's state.
	 */
	UPROPERTY(Transient)
	FSpawnerState SpawnerState{};

	UPROPERTY(Transient)
	bool bIsManuallyStopped = false;

	FTimerHandle SpawnerTimerHandle;

private:
	/**
	 * Callback triggered whenever an enemy dies through the <b>ANpcBase::OnDied</b> delegate.
	 * @param KilledEnemy The victim of the murder.
	 * @param PointsPerKill How many points the kill was worth.
	 * @param Killer The murderer (mainly the human controlling the protagonist).
	 */
	UFUNCTION()
	void OnEnemyKilled(const AActor* KilledEnemy, int32 PointsPerKill, const AActor* Killer);

	void DebugDraw();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void Tick(float DeltaTime) override;
#endif
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Verifies all the necessary conditions to spawn a new group of enemies.
	 */
	FORCEINLINE void TrySpawnGroup();
	
	/**
	 * Spawns a group of enemies.
	 * @param Amount How many enemies to spawn.
	 */
	void SpawnGroup(int Amount);
	
	UFUNCTION()
	void SpawnSingleEnemy();

	/**
	 * Verifies the necessary conditions and if met, updates
	 * the spawner's data for the next phase triggering the <b>OnPhaseCompleted</b> delegate.
	 */
	void UpdatePhase();

	/**
	 * Randomizes the spawn point between the available spawning locations
	 * excluding the last picked.
	 * @return The spawn point location.
	 */
	FVector GetRandomSpawnPoint();
	
	FVector GetRandomSpawnDeviation() const;

	/**
	 * Randomizes the class of enemy that should be spawned using the
	 * corresponding type-weight system. 
	 * @return The enemy class that will be spawned.
	 */
	TSubclassOf<AActor> GetRandomEnemyClass() const;
	
public:
	UFUNCTION(BlueprintCallable)
	void StartSpawner();

	UFUNCTION(BlueprintCallable)
	void StopSpawner();
	
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Spawner Utilities",
		meta = (ToolTip =
			"Click this to spawn a new empty pool."))
	void SpawnPool();

	UFUNCTION(CallInEditor, Category = "Spawner Utilities",
		meta = (ToolTip =
			"Click this to auto-fill spawner arrays."))
	void AutoSetup();
#endif
};
