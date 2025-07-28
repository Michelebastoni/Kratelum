#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

class APool;
class UPoolData;

/**
 * Interface that should be placed on objects that can be contained inside a pool.
 */
UINTERFACE(BlueprintType, Blueprintable)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

class TBO02_API IPoolable
{
	GENERATED_BODY()

public:
	/**
	 * Override this method to perform the poolable object initialization.\n
	 * (Should be called once in the lifetime of the object.)
	 * @param Pool The pool the object is inside.
	 */
	UFUNCTION(BlueprintType, BlueprintNativeEvent, Category = "Pool System")
	void Initialize(APool* Pool);

	/**
	 * Callback triggered whenever a poolable object is picked from its pool.
	 * Override this method to perform additional logic
	 * after the object is picked from its pool.
	 */
	UFUNCTION(BlueprintType, BlueprintNativeEvent, Category = "Pool System")
	void OnObjectPooled();

	/**
	 * Callback triggered whenever a poolable object is returned to the pool.
	 * Override this method to perform additional logic
	 * after the object is sent back to its pool.
	 */
	UFUNCTION(BlueprintType, BlueprintNativeEvent, Category = "Pool System")
	void OnObjectReturnToPool();
	
	/**
	 * Is the current object being used, or is it available to be picked from its pool?
	 * @return The overrider should return <b>true</b> if the object is not available, <b>false</b> otherwise.
	 */
	UFUNCTION(BlueprintType, BlueprintNativeEvent, Category = "Pool System")
	bool GetIsInUse();
};
