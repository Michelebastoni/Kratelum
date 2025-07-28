#pragma once

#include "CoreMinimal.h"
#include "PoolingSystem/Actors/Pool.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "RocketProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitPlayerHit, bool, isHit);

UCLASS()
class TBO02_API ARocketProjectile : public AProjectileBase
{
	GENERATED_BODY()
	
	bool bHasHitPlayer = false;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnHitPlayerHit OnHitPlayerHitDelegate;

	void SetHasHitPlayer(const bool Value) 
	{
		bHasHitPlayer = Value;
		GetWorldTimerManager().ClearTimer(PoolingTimerHandle);
		bIsPooled = false;
		ProjectilePool->ReturnPoolable(this);
	 	OnHitPlayerHitDelegate.Broadcast(bHasHitPlayer);
	};
};
