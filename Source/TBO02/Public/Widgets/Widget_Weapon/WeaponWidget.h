// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWeaponTimeBar> WeaponBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> WeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> WaeponAmmoImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WeaponProjectileNumber;

	UPROPERTY(EditDefaultsOnly)
	FSlateColor weaponLowAmmoColor;
	FSlateColor initialWeaponAmmoColor;

	UPROPERTY()
	TObjectPtr<class AGenericCharacter> GenericCharacter;

	UPROPERTY()
	TObjectPtr<class AGenericPlayerController> Player_Controller;

	UPROPERTY()
	TObjectPtr<class AWeapon> WeaponRef;
	
	UPROPERTY()
	TObjectPtr<class UQTEReload> Reload_Widget;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void UpdateWeaponUI(class AWeapon* newWeapon, const float weaponDuration, bool bIsWeaponBase);
	
	UFUNCTION()
	void UpdateWeaponAmmo(const int currentAmmo);
	
	UFUNCTION()
	void SetUp(const class AGenericPlayerController* Pc);
	
	UFUNCTION()
	void UpdateCurrentWeaponAmmo(const class UQTEReload* reloadWidget);
};
