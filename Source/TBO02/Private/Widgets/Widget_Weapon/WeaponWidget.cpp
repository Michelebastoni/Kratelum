#include "Widgets/Widget_Weapon/WeaponWidget.h"
#include "Widgets/Widget_Weapon/WeaponTimeBar.h"
#include "CharacterLogic/GenericCharacter.h"
#include "Weapons/Weapon.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"

#include "Controllers/GenericPlayerController.h"
#include "Widgets/Widget_Reload/QTEReload.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Player_Controller = Cast<AGenericPlayerController>(GetOwningPlayer());

	checkf(Player_Controller, TEXT("Player_Controller missing on %s"), *GetName());

	Player_Controller->OnFinishSetUp.AddDynamic(this, &ThisClass::SetUp);

	GenericCharacter = Cast<AGenericCharacter>(GetOwningPlayerPawn());

	checkf(GenericCharacter, TEXT("Character not found at weapont widget, ensure that is set correctly when spawn widget, and has the correct controller and pawn"));

	WeaponBar->SetUp(GenericCharacter);

	GenericCharacter->OnChangeWeapon.AddUniqueDynamic(this, &ThisClass::UpdateWeaponUI);

	initialWeaponAmmoColor = WeaponProjectileNumber->GetColorAndOpacity();
}

void UWeaponWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(Player_Controller)
	{
		Player_Controller->OnFinishSetUp.RemoveDynamic(this, &ThisClass::SetUp);
	}

	if(GenericCharacter)
	{
		GenericCharacter->OnChangeWeapon.RemoveDynamic(this, &ThisClass::UpdateWeaponUI);
	}

	if(WeaponRef)
	{
		WeaponRef->onShoot.RemoveDynamic(this, &ThisClass::UpdateWeaponAmmo);
	}

	if(Reload_Widget)
	{
		Reload_Widget->OnNotPressed.RemoveDynamic(this, &ThisClass::UpdateCurrentWeaponAmmo);
	}
}

void UWeaponWidget::UpdateWeaponUI(class AWeapon* newWeapon, const float weaponDuration, bool bIsWeaponBase)
{
	check(newWeapon);

	WeaponRef = newWeapon;

	WeaponRef->onShoot.AddUniqueDynamic(this, &ThisClass::UpdateWeaponAmmo);

	UTexture2D* WeaponTexture = WeaponRef->weaponSprite;

	if(WeaponTexture)
	{
		WeaponIcon->SetBrushFromTexture(WeaponTexture);
	}

	UTexture2D* AmmoTexture = WeaponRef->equippedProjectile->projectileSprite;

	if(AmmoTexture)
	{
		WaeponAmmoImage->SetBrushFromTexture(AmmoTexture);
	}

	UpdateWeaponAmmo(WeaponRef->GetAmmoAmount());
}

void UWeaponWidget::UpdateWeaponAmmo(const int currentAmmo)
{
	if(currentAmmo <= WeaponRef->lowAmmoThreshold)
	{
		WeaponProjectileNumber->SetColorAndOpacity(weaponLowAmmoColor);
	}
	else
	{
		WeaponProjectileNumber->SetColorAndOpacity(initialWeaponAmmoColor);
	}

	FFormatNamedArguments args;
	args.Add("Current Ammo", currentAmmo);
	const FText& ammoNumber =
		FText::Format(NSLOCTEXT(
			"MyNamespace",
			"MyKey",
			"x{Current Ammo}"), args);

	WeaponProjectileNumber->SetText(ammoNumber);

}

void UWeaponWidget::SetUp(const AGenericPlayerController* Pc)
{
	Reload_Widget = Pc->GetCurrentReloadWidget();
	Reload_Widget->OnNotPressed.AddDynamic(this, &ThisClass::UpdateCurrentWeaponAmmo);
}

void UWeaponWidget::UpdateCurrentWeaponAmmo(const UQTEReload* reloadWidget)
{
	if(!WeaponRef)
	{
		return;
	}

	UpdateWeaponAmmo(WeaponRef->GetAmmoAmount());
}
