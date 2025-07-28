#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pool.generated.h"

class ASpawnLocation;
class UPoolData;

DECLARE_LOG_CATEGORY_EXTERN(LogPool, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoolResized, int, NewSize);

/**
 * Class that performs the pooling logic using the given data.
 */
UCLASS()
class TBO02_API APool : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> PooledObjects;

	UPROPERTY(Transient)
	TObjectPtr<UWorld> CachedWorld;

	int32 ResizeTolerance = 20;

	FVector SpawnPos;

protected:
	/**
	 * Config asset containing how the pool should behave.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPoolData> PoolData;

	/**
	 * Where to spawn the poolable objects at start.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> InitialSpawnLocation;

public:
	/**
	 * Utility delegate broadcasted whenever
	 * the pool is forced to resize.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPoolResized OnPoolResized;

private:
	/**
	 * Perform the resize logic.\n
	 * (Intensive operation to perform at runtime, used as last safety measure)
	 */
	void Resize();

	/**
	 * Utility method to check if the given Actor implements the IPoolable interface.
	 * @param Actor The actor to scan.
	 * @return <b>true</b> if the Actor implements the IPoolable interface, <b>false</b> otherwise.
	 */
	bool IsPoolable(const AActor* Actor) const;

public:
	/**
	 * Gets whichever object was configured to be spawned from this pool.
	 * @return The poolable actor that this pool is configured to spawn.
	 */
	TSubclassOf<AActor> GetObjectToSpawn() const;

	/**
	 * Initializes the pool, performing the concrete spawn logic
	 * along with all the necessary checks and triggers initialization
	 * callbacks on all the spawned poolable objects.
	 */
	UFUNCTION(BlueprintCallable)
	void Init();

	/**
	 * Performs the necessary logic to get a poolable Actor from this pool.
	 * @return The poolable Actor if there are any available, a null pointer otherwise. 
	 */
	UFUNCTION(BlueprintCallable)
	AActor* TryGetPoolable();

	/**
	 * Returns a pooled Actor to the pool after it was used.
	 * @param Poolable The Actor to return to the pool.
	 */
	UFUNCTION(BlueprintCallable)
	void ReturnPoolable(AActor* Poolable);
};
