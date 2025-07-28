#include "Weapons/ProjectileMultipool.h"

#include "NPCs/Subsystems/NPCSubsystem.h"
#include "PoolingSystem/Actors/Pool.h"
#include "Weapons/ProjectilePoolingSubsystem.h"
#include "Weapons/Projectiles/ProjectileBase.h"

AProjectileMultipool::AProjectileMultipool()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectileMultipool::BeginPlay()
{
	Super::BeginPlay();
	PoolsMap.Empty();

	for (int32 i = 0; i < ProjectilesPools.Num(); i++)
	{
		ProjectilesPools[i]->Init();

		if (const TSubclassOf<AProjectileBase> Obj = Cast<UClass>(ProjectilesPools[i]->GetObjectToSpawn()))
		{
			if (IsValid(ProjectilesPools[i]))
			{
				PoolsMap.Add(Obj.Get(), ProjectilesPools[i]);
				UE_LOG(LogTemp, Display, TEXT("Registered pool for: %s"), *Obj->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Pool is invalid when registering: %s"), *Obj->GetName());
			}
		}
	}
	UProjectilePoolingSubsystem::GetInstance(GetWorld())->SetPool(this);
}

AProjectileBase* AProjectileMultipool::GetProjectileByType(TSubclassOf<AProjectileBase> Type)
{
	if (!Type)
	{
		UE_LOG(LogTemp, Error, TEXT("Projectile Type is nullptr!"));
		return nullptr;
	}
	
	if (PoolsMap.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("PoolsMap empty!"));
		return nullptr;
	}
	
	// Search compatible key manually
	for (const TPair<UClass*, APool*>& Elem : PoolsMap)
	{
		if (Type->IsChildOf(Elem.Key))
		{
			if (IsValid(Elem.Value))
			{
				return Cast<AProjectileBase>(Elem.Value->TryGetPoolable());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Pool for projectile %s is no longer valid!"), *Type->GetName());
				return nullptr;
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("No pool found for projectile type: %s"), *Type->GetName());
	return nullptr;
}

#if WITH_EDITOR
void AProjectileMultipool::AddPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (const auto Pool = World->SpawnActor<APool>(); !ProjectilesPools.Contains(Pool))
	{
		ProjectilesPools.Add(Pool);
	}
}
#endif