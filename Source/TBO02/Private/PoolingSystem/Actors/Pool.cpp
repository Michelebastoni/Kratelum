#include "PoolingSystem/Actors/Pool.h"

#include "PoolingSystem/Data/PoolData.h"
#include "PoolingSystem/Interfaces/Poolable.h"

DEFINE_LOG_CATEGORY(LogPool);

void APool::Init()
{
	checkf(PoolData, TEXT("Missing pool data on pool [%s]!"), *GetNameSafe(this));
	checkf(InitialSpawnLocation, TEXT("Missing initial spawn location on pool [%s]!"), *GetNameSafe(this));

	CachedWorld = GetWorld();

	if (!CachedWorld)
	{
		UE_LOG(LogPool, Error, TEXT("World is null! Cannot Init Pool!"));
		return;
	}

	if (!PoolData->ObjectToSpawn->ImplementsInterface(UPoolable::StaticClass()))
	{
		UE_LOG(LogPool, Error, TEXT("Object to pool is not a Poolable!"));
		return;
	}

	PooledObjects.Empty();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnPos = InitialSpawnLocation->GetActorLocation();

	// Spawn all the objects and fill the pool.
	for (int32 i = 0; i < PoolData->PoolSize; i++)
	{
		AActor* Obj = CachedWorld->SpawnActor<AActor>(PoolData->ObjectToSpawn,
		                                              SpawnPos,
		                                              FRotator::ZeroRotator, SpawnParams);

		if (!Obj)
		{
			UE_LOG(LogPool, Error, TEXT("Failed to spawn actor from class %s"), *PoolData->ObjectToSpawn->GetName());
			continue;
		}

		if (!IsValid(Obj))
		{
			UE_LOG(LogPool, Error, TEXT("INVALID OBJ ON POOL %s"), *this->GetName());
			return;
		}

		PooledObjects.Add(Obj);

		if (!Obj->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			UE_LOG(LogPool, Error, TEXT("Spawned actor does not implement UPoolable!"));
			continue;
		}

		IPoolable::Execute_Initialize(Obj, this);
	}
}

TSubclassOf<AActor> APool::GetObjectToSpawn() const
{
	return PoolData->ObjectToSpawn;
}

AActor* APool::TryGetPoolable()
{
	// Find and return the first available poolable
	for (int32 i = 0; i < PooledObjects.Num(); i++)
	{
		if (!IsPoolable(PooledObjects[i]))
		{
			continue;
		}

		if (!IPoolable::Execute_GetIsInUse(PooledObjects[i]))
		{
			IPoolable::Execute_OnObjectPooled(PooledObjects[i]);
			return PooledObjects[i];
		}
	}

	// Try resize logic if the pool has no more available objects.
	if (PoolData->bCanResize)
	{
		Resize();

		// Re-run availability logic and return the newly first available object.
		for (AActor* PooledActor : PooledObjects)
		{
			if (!IsPoolable(PooledActor))
			{
				continue;
			}

			if (!IPoolable::Execute_GetIsInUse(PooledActor))
			{
				IPoolable::Execute_OnObjectPooled(PooledActor);
				return PooledActor;
			}
		}

		UE_LOG(LogPool, Error, TEXT("Resize attempted but still no actors available in pool [%s]"), *GetNameSafe(this));
	}
	else
	{
		UE_LOG(LogPool, Warning, TEXT("Pool [%s] spawning [%s] is empty and cannot expand!"), *GetNameSafe(this), *GetObjectToSpawn()->GetName());
	}
	return nullptr; // Everything went wrong, we are forced to return a null pointer.
}

void APool::ReturnPoolable(AActor* Poolable)
{
	if (!IsPoolable(Poolable))
	{
		return;
	}

	// Reset the poolable position to avoid possible collisions
	Poolable->SetActorLocation(SpawnPos);

	IPoolable::Execute_OnObjectReturnToPool(Poolable);
}

bool APool::IsPoolable(const AActor* Actor) const
{
	if (!Actor || !IsValid(Actor) || Actor->IsPendingKillPending())
	{
		UE_LOG(LogPool, Error, TEXT("Actor of type [%s] dequeued from pool [%s] is invalid or pending kill!"), *GetNameSafe(PoolData->ObjectToSpawn), *GetNameSafe(this));
		return false;
	}

	if (!Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		UE_LOG(LogPool, Error, TEXT("[%s] Dequeued actor does not implement UPoolable!"), *Actor->GetName());
		return false;
	}
	return true;
}

void APool::Resize()
{
	UE_LOG(LogPool, Warning, TEXT("Resize pool invoked")); //, maybe you should revise your economy?
	// UE_LOG(LogPool, Warning, TEXT("----- [%d] -----"), ResizeTolerance);
	// ResizeTolerance--;
	//
	// // Totally hardcoded value to punish irresponsible designers
	// checkf(ResizeTolerance != 0,
	//        TEXT(
	// 	       "You clearly did not set up the pool amount correctly and called its resize too many times! Review your economy!"
	//        ));

	if (!CachedWorld || !PoolData->ObjectToSpawn ||
		!PoolData->ObjectToSpawn->ImplementsInterface(UPoolable::StaticClass()))
	{
		UE_LOG(LogPool, Error, TEXT("Cannot expand pool [%s]: invalid world or class"), *this->GetName());
		return;
	}

	if (PooledObjects.Num() + PoolData->ResizeAmount > PoolData->MaxPoolSize)
	{
		UE_LOG(LogPool, Error, TEXT("Cannot expand pool [%s]: max size reached (%d)"), *this->GetName(),
		       PoolData->MaxPoolSize);
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < PoolData->ResizeAmount; i++)
	{
		AActor* Actor = CachedWorld->SpawnActor<AActor>(PoolData->ObjectToSpawn, FVector::ZeroVector,
		                                                FRotator::ZeroRotator,
		                                                SpawnParams);

		if (!Actor || !Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			UE_LOG(LogPool, Error, TEXT("Failed to spawn valid actor while expanding pool [%s]"), *this->GetName());
			continue;
		}

		IPoolable::Execute_Initialize(Actor, this);
		PooledObjects.Add(Actor);
	}

	OnPoolResized.Broadcast(PooledObjects.Num());
}
