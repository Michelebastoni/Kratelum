// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectileDataAsset.generated.h"

/**
 * 
 */

class UModifierBase;
class AProjectileBase;

UCLASS()
class TBO02_API UProjectileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectileBase> projectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float projectileSpeed = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int damage = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Units = "m"))
	float projectileRange = 1000;
	
	UPROPERTY(editanywhere, BlueprintReadWrite)
	bool penetrating = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float stunTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* gunshot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* impactSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* bulletFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float loudness = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float impactPower = 1000.0f;

	UPROPERTY(EditAnywhere, Category="Weapon", DisplayName="Projectile Sprite")
	UTexture2D* projectileSprite;
	
	virtual void Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction, TArray<FHitResult>& hitResults, class AWeapon*
	                              instigatorWeapon) { }

protected:
	UFUNCTION()
	virtual void CallWeaponApplyDamage(AActor* actor, FHitResult hitResult, AWeapon* weapon);
	
private:
};
