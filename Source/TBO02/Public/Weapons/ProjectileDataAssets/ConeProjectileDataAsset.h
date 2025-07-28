// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "ConeProjectileDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UConeProjectileDataAsset : public UProjectileDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float coneAngle = 45;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int bulletAmount = 10;

	virtual void Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction, TArray<FHitResult>& hitResults, AWeapon*
	                              instigatorWeapon) override;
};
