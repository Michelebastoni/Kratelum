#include "Widgets/Widget_Dash/DashesWidget.h"
#include "Widgets/Widget_Dash/DashBar.h"

#include "Controllers/GenericPlayerController.h"
#include "Controllers/Actions/DashAction.h"

#include "CharacterLogic/PlayerCharacter.h"

#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"

#include "Kismet/GameplayStatics.h"

void UDashesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	genericCharacter = Cast<AGenericCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!genericCharacter)
	{
		return;
	}
	
	genericController = Cast<AGenericPlayerController>(genericCharacter->GetController());
	if (!genericController)
	{
		return;
	}
	
	DashAction = genericController->GetAction<UDashAction>();
	
	checkf(DashAction, TEXT("Dash Action data is null into %s"), *GetNameSafe(this));

	DashAction->OnDashPerformed.AddDynamic(this, &UDashesWidget::OnChangeDash);
	DashAction->OnDashCooldownEnd.AddDynamic(this, &UDashesWidget::OnChangeDash);
	DashAction->OnDashCooldownEnd.AddDynamic(this, &UDashesWidget::Notify_DashEnd);
	DashAction->OnDashCooldownChanged.AddDynamic(this, &UDashesWidget::OnChangeDashCooldown);
	
	
	DashCoolDown = DashAction->GetDashCoolDown();
	CreateDashesBar();
	
}

void UDashesWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(DashAction)
	{
		DashAction->OnDashPerformed.RemoveDynamic(this, &UDashesWidget::OnChangeDash);
		DashAction->OnDashCooldownEnd.RemoveDynamic(this, &UDashesWidget::OnChangeDash);
		DashAction->OnDashCooldownEnd.RemoveDynamic(this, &UDashesWidget::Notify_DashEnd);
		DashAction->OnDashCooldownChanged.RemoveDynamic(this, &UDashesWidget::OnChangeDashCooldown);
	}
}

void UDashesWidget::CreateDashesBar()
{
	UDashBar* dashBar = nullptr;
	int32 MaxDashCount = DashAction->GetMaxDashCount();

	for (int32 i = 1; i <= MaxDashCount; i++)
	{
		dashBar = CreateWidget<UDashBar>(this, DashBarClass);
		if (dashBar)
		{
			dashBar->ForceSetProgressBar(1.f);
			HorizontalBox->AddChildToHorizontalBox(dashBar);
			Dashes.Add(dashBar);
			dashBar->SetProgressBarDuration(DashCoolDown);
		}
	}
}

void UDashesWidget::OnChangeDash(int DashIndex)
{
	// Check if dashIndex is valid
	if(DashIndex < 0 || DashIndex > Dashes.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("DashIndex is not valid \n Source: [%s]"), *GetNameSafe(this));
		return;
	}

	// If Dash_bar index is equal to array it mean is full
	if(DashIndex == Dashes.Num())
	{
		return;
	}

	// If is the previous dash_bar
	if(DashIndex < CurrentDashIndex)
	{
		float CurrentDash_BarPercent = 0.f;
		float CurrentDashFillDuration = 0.0f;

		// Take the index of the next dash_bar
		int32 NextDashBarIndex = DashIndex + 1;

		// If next dash_bar index is valid
		if(NextDashBarIndex < Dashes.Num())
		{
			// Save the previous dash_bar percent value
			CurrentDash_BarPercent = Dashes[NextDashBarIndex]->GetCurrentProgressBarPercent();

			// Set the current dash_bar fill duration
			CurrentDashFillDuration = Dashes[NextDashBarIndex]->GetProgressBarDuration();

			// Set next dash_bar value to 0.0f
			Dashes[NextDashBarIndex]->ForceSetProgressBar(0.0f);

			// Set current dash bar value to the previous bar value
			Dashes[DashIndex]->ForceSetProgressBar(CurrentDash_BarPercent);

			Dashes[DashIndex]->SetProgressBarDuration(DashAction->GetDashCoolDownTimer());
		}
	}
	else
	{
		// Set dash_bar value to 0.0f
		Dashes[DashIndex]->ForceSetProgressBar(0.0f);
		
		// Set dash_bar fill duration
		Dashes[DashIndex]->SetProgressBarDuration(DashAction->GetDashCoolDownTimer());
	}

	// Set current dash_bar index
	CurrentDashIndex = DashIndex;

	// Start Refill dash_bar
	Dashes[CurrentDashIndex]->StartToRefill();
	
}

void UDashesWidget::OnChangeDashCooldown(const UDashAction* currentDashAction)
{
	if(!Dashes[CurrentDashIndex])
	{
		UE_LOG(LogTemp, Error, TEXT("Dashes bar UI Index invalid into %d"), *GetNameSafe(this));
		return;
	}

	float CurrentDashPercent = Dashes[CurrentDashIndex]->GetCurrentProgressBarPercent();
	float CurrentFillBarDuration = Dashes[CurrentDashIndex]->GetProgressBarDuration();
	float NewDashCooldown = currentDashAction->GetDashCoolDown();

	float DeltaFillDuration = NewDashCooldown * CurrentDashPercent;
	float NewFillDuration = NewDashCooldown - DeltaFillDuration;

	float sample = currentDashAction->GetDashCoolDownTimer();
	Dashes[CurrentDashIndex]->SetProgressBarDuration(sample);
	Dashes[CurrentDashIndex]->ForceSetProgressBar(CurrentDashPercent);
	Dashes[CurrentDashIndex]->StartToRefill();

	/*UE_LOG(LogTemp, Log, TEXT("CurrentDashPercent: %.2f"), CurrentDashPercent);
	UE_LOG(LogTemp, Log, TEXT("CurrentFillBarDuration: %.2f"), CurrentFillBarDuration);
	UE_LOG(LogTemp, Log, TEXT("NewDashCooldown: %.2f"), NewDashCooldown);
	UE_LOG(LogTemp, Log, TEXT("DeltaFillDuration: %.2f"), DeltaFillDuration);
	UE_LOG(LogTemp, Log, TEXT("NewFillDuration: %.2f"), NewFillDuration);*/
}
