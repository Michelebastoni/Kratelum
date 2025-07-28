#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Queue.h"
#include "PoolData.generated.h"

/**
 * Config asset class used to describe how a pool should behave.
 */
UCLASS(BlueprintType)
class TBO02_API UPoolData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * How many objects should the pool spawn at the BeginPlay?
	 */
	UPROPERTY(EditAnywhere)
	int32 PoolSize;

	/**
	 * Which object should the pool be filled with?
	 */
	UPROPERTY(EditAnywhere, meta = (MustImplement = "/Script/TBO02.Poolable"))
	TSubclassOf<AActor> ObjectToSpawn;

	/**
	 * Can this pool resize?\n
	 * (Note: it can be a really resource-intensive operation to perform at runtime!)
	 */
	UPROPERTY(EditAnywhere)
	bool bCanResize = false;

	/**
	 * If the pool is able to resize, which should be the actual limit?
	 */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanResize", ClampMin = "1"))
	int32 MaxPoolSize = 1;

	/**
	 * By how many elements should the pool resize per iteration?
	 */
	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "bCanResize", ClampMin = "1", ClampMax = "5", UIMin = "1", UIMax = "5"))
	int32 ResizeAmount = 1;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
