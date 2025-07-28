// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponParametersDataAsset.generated.h"

class UNiagaraSystem;
class UProjectileDataAsset;
class USkeletalMesh;
/**
 * 
 */
UCLASS()
class TBO02_API UWeaponParametersDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponName;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Magazine Size")
	int magSize = 30;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Time Between Shots")
	float shootingCooldown = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="QTE Overflow Value")
	int overflowValue = 0;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Low Ammo Threshold")
	int lowAmmoThreshold = 0;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Weapon Sprite")
	TObjectPtr<class USkeletalMesh> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Weapon Sprite")
	TObjectPtr<class UTexture2D> weaponSprite;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Projectile")
	TObjectPtr<class UProjectileDataAsset> projectile;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> impactKillSound;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> muzzleFlash;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> ShootingMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> ReloadMontage;
};
