// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "StraightProjectileDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UStraightProjectileDataAsset : public UProjectileDataAsset
{
	GENERATED_BODY()
	
	virtual void Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction, TArray<FHitResult>& hitResults, AWeapon*
	                              instigatorWeapon) override;

};
