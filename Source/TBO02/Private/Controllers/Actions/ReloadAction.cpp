#include "Controllers/Actions/ReloadAction.h"
#include "Controllers/GenericPlayerController.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "OverflowSystem/Overflow.h"
#include "Weapons/Weapon.h"
#include "Widgets/Widget_Reload/QTEReload.h"

void UReloadAction::SetUp(AGenericPlayerController* controller)
{
	Super::SetUp(controller);

	SetUpData();

	if(reloadReleaseAction)
	{
		playerController->GetEnhancedInputComponent()->BindAction(
			reloadReleaseAction,
			releaseEventTrigger,
			this,
			&ThisClass::ReleaseReload
		);

	}

	reloadWidget = playerController->GetCurrentReloadWidget();

	if (reloadWidget)
	{
		reloadWidget->OnHitOverflowZone.AddDynamic(this, &ThisClass::OnReloadSuccess);
		reloadWidget->OnMissOverflowZone.AddDynamic(this, &ThisClass::OnReloadFailed);
		reloadWidget->OnReloadEnd.AddDynamic(this, &ThisClass::OnReloadEnd);
	}
	
}

void UReloadAction::ShutDown()
{
	Super::ShutDown();

	if (reloadWidget)
	{
		reloadWidget->OnHitOverflowZone.RemoveDynamic(this, &ThisClass::OnReloadSuccess);
		reloadWidget->OnMissOverflowZone.RemoveDynamic(this, &ThisClass::OnReloadFailed);
		reloadWidget->OnReloadEnd.RemoveDynamic(this, &ThisClass::OnReloadEnd);
	}
}

void UReloadAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if(!Player->GetCurrentWeapon())
	{
		UE_LOG(LogTemp, Error, TEXT("Cant reload due there's not a weapon"));
		return;
	}

	if (Player->GetCurrentWeapon()->CanReload() == CanReloadResponses::CanReload)
	{	
		if(!reloadTimer.IsValid())
		{
			world->GetTimerManager().SetTimer(
				reloadTimer,
				this,
				&ThisClass::PerformReload,
				ReloadData.ReloadDelay,
				false
			);
		}
	}
}

void UReloadAction::OnReloadSuccess(const UQTEReload* reloadQTEWidget)
{
	if (world && ReloadData.Success_Reload_SFX)
	{
		UGameplayStatics::PlaySound2D(world, ReloadData.Success_Reload_SFX);
	}

	bSound_Already_Play = true;

	Player->GetOverflowComponent()->AddOverflowWithoutSound(ReloadData.PerfectReloadOveflowValue);
}

void UReloadAction::OnReloadFailed(const UQTEReload* reloadQTEWidget)
{
	if (world && ReloadData.Failed_Reload_SFX)
	{
		UGameplayStatics::PlaySound2D(world, ReloadData.Failed_Reload_SFX);
		bSound_Already_Play = true;
	}
}

void UReloadAction::OnReloadEnd(const UQTEReload* reloadQTEWidget)
{
	if(!bSound_Already_Play)
	{
		OnReloadFailed(reloadQTEWidget);
	}

	bSound_Already_Play = false;
}

void UReloadAction::PerformReload()
{
	float animationDuration = 0.0f;
	Player->GetCurrentWeapon()->StartReload();

	// Play Start Reload SFX
	if(ReloadData.Start_Reload_SFX && world)
	{
		UGameplayStatics::PlaySound2D(world, ReloadData.Start_Reload_SFX);
	}

	// Handle Reload Animation
	if (Player->GetCurrentWeapon()->GetCharacterReloadAnimationMontage())
	{
		currentReloadMontage = Player->GetCurrentWeapon()->GetCharacterReloadAnimationMontage();
		Player->PlayAnimMontage(currentReloadMontage);
		animationDuration =
			currentReloadMontage->GetPlayLength() /
			Player->GetMesh()->GetAnimInstance()->Montage_GetEffectivePlayRate(currentReloadMontage);
	}

	// Handle Reload UI
	if (reloadWidget)
	{
		if (reloadWidget->GetCanSliderStart())
		{
			reloadWidget->StartReloadSlider(animationDuration);
		}
	}
	else
	{
		// DEBUG MESSAGE
		UE_LOG(LogTemp, Error, TEXT("Reload Widget not set into controller"));
	}

	world->GetTimerManager().ClearTimer(reloadTimer);
		
}

void UReloadAction::ReleaseReload()
{
	if (Player->GetCurrentWeapon()->CanReload() == CanReloadResponses::CanReload)
	{
		return;
	}

	if (!reloadWidget)
	{
		return;
	}

	if (Player->GetCurrentWeapon()->IsReloading())
	{
		reloadWidget->GetResult();

		Player->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(currentReloadMontage, 999.0f);
	}
}

void UReloadAction::SetUpData()
{
	if(Failed_Reload_SFX)
	{
		ReloadData.Failed_Reload_SFX = Failed_Reload_SFX;
	}

	if(Start_Reload_SFX)
	{
		ReloadData.Start_Reload_SFX = Start_Reload_SFX;
	}

	if(Success_Reload_SFX)
	{
		ReloadData.Success_Reload_SFX = Success_Reload_SFX;
	}

	ReloadData.PerfectReloadOveflowValue = perfectReloadOveflowValue;
	ReloadData.ReloadDelay = reloadDelay;
}
