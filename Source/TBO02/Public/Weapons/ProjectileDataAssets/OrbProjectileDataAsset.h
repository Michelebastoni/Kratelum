// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "OrbProjectileDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UOrbProjectileDataAsset : public UProjectileDataAsset
{
	GENERATED_BODY()

public:

private:

public:
	virtual void Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction, TArray<FHitResult>& hitResults, AWeapon*
	                              instigatorWeapon) override;

private:

};
