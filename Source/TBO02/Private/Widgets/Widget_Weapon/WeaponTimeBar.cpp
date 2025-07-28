#include "Widgets/Widget_Weapon/WeaponTimeBar.h"
#include "Components/ProgressBar.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Weapons/Weapon.h"

void UWeaponTimeBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProgressBar)
	{
		defaultWeaponColor = ProgressBar->GetFillColorAndOpacity();
		ProgressBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWeaponTimeBar::SetUp(AGenericCharacter* characterRef)
{
	checkf(characterRef, TEXT("Passed a null reference on Weapon timer bar setup"));

	GenericCharacter = characterRef;

	GenericCharacter->OnChangeWeapon.AddUniqueDynamic(this, &UWeaponTimeBar::OnPickUpNewWeapon);
}

void UWeaponTimeBar::NativeDestruct()
{
	Super::NativeDestruct();

	if (GenericCharacter)
	{
		GenericCharacter->OnChangeWeapon.RemoveDynamic(this, &ThisClass::OnPickUpNewWeapon);
	}
}

void UWeaponTimeBar::OnPickUpNewWeapon(AWeapon* newWeapon,const float weaponDuration, bool bIsBaseWeapon)
{
	if (!ProgressBar)
	{
		return;
	}

	if(bIsBaseWeapon)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	ProgressBar->SetVisibility(ESlateVisibility::Visible);
	ForceSetProgressBar(1.f);
	changeProgressBarDuration = weaponDuration;
	SetProgressBarValue(0.f);
}

void UWeaponTimeBar::CheckProgressBarStatus()
{
	Super::CheckProgressBarStatus();
	
	if (!ProgressBar)
	{
		return;
	}
	
	if (currentProgressBarPercent <= 0.f)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (currentProgressBarPercent < lowWeaponRatio)
	{
		ProgressBar->SetFillColorAndOpacity(lowWeaponColor);
	}
	else
	{
		ProgressBar->SetFillColorAndOpacity(defaultWeaponColor);
	}
}
