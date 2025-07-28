#include "Widgets/Widget_Reload/QTEReload.h"
#include "Components/Slider.h"
#include "Components/CanvasPanelSlot.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Controllers/GenericPlayerController.h"
#include "Controllers/Actions/ReloadAction.h"

void UQTEReload::NativeConstruct()
{	
	if (!IsValid(reloadSliderMat))
	{
		Super::NativeConstruct();
		return;
	}

	Player_Controller = Cast<AGenericPlayerController>(GetOwningPlayer());

	bCanSliderStart = true;
	bIsPressed = false;
	bSuccess = false;

	// Create dynamic material
	matInstance =
		UMaterialInstanceDynamic::Create(reloadSliderMat, this, "ReloadSliderMat");

	// Set material into slider
	FSliderStyle newStyle = reloadSlider->GetWidgetStyle(); // Create new slider style
	newStyle.NormalBarImage.SetResourceObject(matInstance); // Set dynamic material into the newStyle
	reloadSlider->SetWidgetStyle(newStyle); // Set style into the slider

	UE_LOG(LogTemp, Display, TEXT("Reload overflow trigger areas values: \n minValue: %.2f \n maxValue: %.2f"), overflowTriggerZoneMinValue, overflowTriggerZoneMaxValue)

	SetVisibility(ESlateVisibility::Hidden);

	Super::NativeConstruct();
}

void UQTEReload::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bSuccess)
	{
		return;
	}

	if(endTime < GetWorld()->GetTimeSeconds())
	{
		EndReloadSlider();
	}
	else
	{
		// Interpolate slider 0.0f --> 1.0f based on the reload animation
		float sliderPercent =
			FMath::GetMappedRangeValueClamped(
				FVector2D{ startTime, endTime },
				FVector2D{ 0.0f, 1.0f },
				GetWorld()->GetTimeSeconds()
			);

		reloadSlider->SetValue(sliderPercent);
	}
}

/// <summary>
/// function that checks if the player is in the range of the reload slider trigger area
/// </summary>
/// <returns> return true if player is in range, otherwise false</returns>

bool UQTEReload::GetResult()
{
	if(bIsPressed)
	{
		UE_LOG(LogTemp, Error, TEXT("Already Pressed!"));
		return false;
	}

	bIsPressed = true;

	float currentSliderValue = reloadSlider->GetValue();

	// if slider value is inside overflow trigger zone
	if(currentSliderValue > overflowTriggerZoneMinValue && 
		currentSliderValue < overflowTriggerZoneMaxValue)
	{
		OnHitOverflowZone.Broadcast(this);
		
		StartDisappearTimer();

		bSuccess = true;

		return true;
	}
	else
	{
		OnMissOverflowZone.Broadcast(this);
	
		EndReloadSlider();

		return false;
	}
}

/// <summary>
/// Funciton that set the trigger zone (minValue and maxValue) of the reload slider
/// </summary>
/// <param name="minValue"></param>
/// <param name="maxValue"></param>

void UQTEReload::SetTriggerZone(float& currentMinValue, float& currentMaxValue, float minValue, float maxValue, FName minValueName, FName maxValueName)
{
	currentMinValue = minValue;
	currentMaxValue = maxValue;

	matInstance->SetScalarParameterValue(minValueName, currentMinValue);
	matInstance->SetScalarParameterValue(maxValueName, currentMaxValue);
}

// Stop and reset the reload slider
void UQTEReload::EndReloadSlider()
{
	endTime = 0.0f;

	bCanSliderStart = true;

	if(!bIsPressed)
	{
		OnNotPressed.Broadcast(this);
	}

	UE_LOG(LogTemp, Display, TEXT("End Slider"))

	SetVisibility(ESlateVisibility::Hidden);

	OnReloadEnd.Broadcast(this);
}

/// <summary>
/// Start reload slider action, that set initial slider value and set the variable to start the slider interpolation 
/// </summary>
/// <param name="reloadDuration"> the duration of interpolation </param>

void UQTEReload::StartReloadSlider(float reloadDuration)
{
	if (!bCanSliderStart)
	{
		return;
	}

	UWorld* world = GetWorld();

	// Set deafaul var
	bCanSliderStart = false;

	bIsPressed = false;

	bSuccess = false;

	// Clear Timer
	world->GetTimerManager().ClearTimer(DisappearTimer);

	reloadSlider->SetValue(0.0f);

	// Set value to start the reload slider interpolation
	startTime = world->GetTimeSeconds();
	endTime = startTime + reloadDuration;

	SetOverflowTriggerZone(overflowTriggerZoneMinValue, overflowTriggerZoneMaxValue);
	
	UE_LOG(LogTemp, Display, TEXT("Start Slider"))

	SetVisibility(ESlateVisibility::HitTestInvisible);

	OnReloadStart.Broadcast(this);
}

/// <summary>
/// Updat the slider position on screen based on the target
/// </summary>
/// <param name="target"> target ref where slider has to set his position </param>
void UQTEReload::UpdateSliderLocationAtLocation(FVector target)
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	FVector2D newPosition;
	FVector targetLocation = FVector{
		target.X + reloadSliderOffset.X,
		target.Y,
		target.Z + reloadSliderOffset.Y };

	playerController->ProjectWorldLocationToScreen(
		targetLocation,
		newPosition
	);

	newPosition /= UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	UCanvasPanelSlot* canvasPanel = Cast<UCanvasPanelSlot>(reloadSlider->Slot);

	if(canvasPanel)
	{
		canvasPanel->SetPosition(newPosition);
	}

}

void UQTEReload::StartDisappearTimer()
{
	UWorld* world = GetWorld();

	if(!world)
	{
		return;
	}

	FTimerManager& tm = world->GetTimerManager();

	// Clear Timer
	if(DisappearTimer.IsValid())
	{
		tm.ClearTimer(DisappearTimer);
	}

	tm.SetTimer(
		DisappearTimer,
		this,
		&ThisClass::EndReloadSlider,
		Time_ToDisappear
	);
}
