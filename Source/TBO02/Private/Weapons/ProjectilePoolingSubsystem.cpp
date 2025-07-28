#include "Weapons/ProjectilePoolingSubsystem.h"

#include "PoolingSystem/Actors/Pool.h"
#include "Weapons/ProjectileMultipool.h"

UProjectilePoolingSubsystem* UProjectilePoolingSubsystem::GetInstance(const UWorld* World)
{
	if (World)
	{
		return World->GetSubsystem<UProjectilePoolingSubsystem>();
	}
	return nullptr;
}

AProjectileBase* UProjectilePoolingSubsystem::GetProjectileByType(TSubclassOf<AProjectileBase> Type) const
{
	if (!ProjectilePool)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing projectiles multipool!"));
		return nullptr;
	}
	return ProjectilePool->GetProjectileByType(Type);
}

void UProjectilePoolingSubsystem::SetPool(AProjectileMultipool* Value)
{
	ProjectilePool = Value;
}
