// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWeaponParametersDataAsset;
class UImage;
class UNiagaraSystem;
class UMeleeComponent;
class UProjectileDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShoot, const int, bullets);

UENUM(BlueprintType)
enum class CanShootResponses : uint8
{
	CanShoot,
	HasNoAmmo,
	HasNoProjectile,
	IsReloading,
	WeaponCooldown
};

UENUM(BlueprintType)
enum class CanReloadResponses : uint8
{
	CanReload,
	AlreadyReloading,
	StillHasBullets
};

UCLASS()
class TBO02_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Weapon")
	TObjectPtr<class UWeaponParametersDataAsset> WeaponData;

	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="Magazine Size")
	int magSize = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="Time Between Shots")
	float shootingCooldown = 0.5f;
	
	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="QTE Overflow Value")
	int overflowValue = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="Low Ammo Threshold")
	int lowAmmoThreshold = 0;
	
	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	UProjectileDataAsset* equippedProjectile;
	
	UPROPERTY(BlueprintReadWrite, Category="Weapon", DisplayName="Weapon Sprite")
	UTexture2D* weaponSprite;
	
	UPROPERTY(BlueprintReadWrite)
	AActor* weaponHolder = nullptr;
	
	UPROPERTY(BlueprintAssignable)
	FOnShoot onShoot;

protected:
	UPROPERTY(BlueprintReadWrite)
	USoundBase* impactKillSound;
	
	UPROPERTY(BlueprintReadWrite)
	UNiagaraSystem* muzzleFlash;
	
	UPROPERTY(BlueprintReadWrite)
	UAnimMontage* shootAnimationMontage;
	
	UPROPERTY(BlueprintReadWrite)
	UAnimMontage* reloadAnimationMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> muzzleLocation;
	
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* weaponMesh;
	
	UPROPERTY()
	bool isReloading = false;
	
	UPROPERTY(BlueprintReadWrite)
	FVector MuzzlePos = FVector{};
	
	UPROPERTY(BlueprintReadWrite, Transient)
	FVector MuzzleForward = FVector{};
	
	UPROPERTY()
	UMeleeComponent* MeleeComponent = nullptr;
	
	int projectilesInMag;

	bool bCanShoot = true;

private:
	UPROPERTY()
	float lastShotTime = 0;

public:
	AWeapon();

	UFUNCTION(BlueprintCallable)
	UWeaponParametersDataAsset* GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintCallable)
	FVector GetMuzzleLocation() const { return MuzzlePos; };
	UFUNCTION(BlueprintCallable)
	FVector GetMuzzleForward() const { return MuzzleForward; };
	UFUNCTION(BlueprintCallable)
	USceneComponent* GetMuzzle() const { return muzzleLocation; };
	UFUNCTION(BlueprintCallable)
	bool ShootStraightAhead(float randomDeviation = 0.0f);
	UFUNCTION(BlueprintCallable)
	bool ShootTowards(const FVector& targetLocation, float randomDeviation = 0.0f);
	UFUNCTION(BlueprintCallable)
	bool Shoot(FVector direction, float randomDeviation = 0.0f);
	UFUNCTION(BlueprintCallable)
	bool StartReload(UProjectileDataAsset* newProjectile = nullptr);
	UFUNCTION(BlueprintCallable)
	void ApplyReload();
	UFUNCTION(BlueprintCallable)
	void FinishReload();
	UFUNCTION(BlueprintCallable)
	bool IsReloading() const { return isReloading; }

	UFUNCTION(BlueprintCallable)
	CanReloadResponses CanReload();
	
	UFUNCTION(BlueprintCallable)
	CanShootResponses CanShoot();
	
	UFUNCTION(BlueprintCallable)
	bool ApplyDamage(AActor* hitActor, FHitResult hitResult, UProjectileDataAsset* projectileDataAsset);
	
	UFUNCTION(BlueprintCallable)
	int GetAmmoAmount() { return projectilesInMag; }

	UFUNCTION(BlueprintCallable)
	float GetAmmoRate() const {return float(projectilesInMag) / magSize; }

	UFUNCTION(BlueprintCallable)
	void EquipProjectile(UProjectileDataAsset* newProjectile) { equippedProjectile = newProjectile; }
	
	UFUNCTION(BlueprintCallable)
	void SetNewWeaponParameters(UWeaponParametersDataAsset* newParameters);

	UFUNCTION()
	void SetUpData();

	UFUNCTION(BlueprintCallable)
	void ToggleCanShoot(bool bNewValue) { bCanShoot = bNewValue; }

	//Animations
	UAnimMontage* GetCharacterShootAnimationMontage() const { return shootAnimationMontage; }
	UAnimMontage* GetCharacterReloadAnimationMontage() const { return reloadAnimationMontage; }

	UFUNCTION(BlueprintCallable)
	UMeleeComponent* GetMeleeComponent() const
	{
		return MeleeComponent;
	}

	UFUNCTION(BlueprintCallable)
	void SetMeleeComponent(UMeleeComponent* const NewMeleeComponent)
	{
		this->MeleeComponent = NewMeleeComponent;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
};
