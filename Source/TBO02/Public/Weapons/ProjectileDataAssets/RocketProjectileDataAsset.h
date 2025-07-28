#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "RocketProjectileDataAsset.generated.h"


class ARocketProjectile;

UCLASS()
class TBO02_API URocketProjectileDataAsset : public UProjectileDataAsset
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	UWorld* World;
	
	UPROPERTY(Transient)
	ARocketProjectile* projectile;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> ExplosionVfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> LayersToHit;

	virtual void Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction,
						TArray<FHitResult>& hitResults, class AWeapon* instigatorWeapon) override;

protected:
	UFUNCTION()
	void Explode(AActor* actor, FHitResult hitResult, AWeapon* weapon);

	FCollisionObjectQueryParams SetCollisionObjectParams(
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes);
	
	
};
