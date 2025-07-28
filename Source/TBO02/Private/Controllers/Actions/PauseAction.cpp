#include "Controllers/Actions/PauseAction.h"
#include "Controllers/GenericPlayerController.h"
#include "Widgets/GameScreen.h"

void UPauseAction::SetUp(AGenericPlayerController* controller)
{
	Super::SetUp(controller);

	CurrentPauseWidget = playerController->GetCurrentPauseWidget();

	checkf(CurrentPauseWidget, TEXT("Cant find Pause widget on %s, be sure to set it into %s"), *GetNameSafe(this), *GetNameSafe(playerController));
	
}


void UPauseAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if(CurrentPauseWidget->GetIsGameScreenActive())
	{
		CurrentPauseWidget->ToggleGameScreen(false);
	}
	else
	{
		CurrentPauseWidget->ToggleGameScreen(true);
	}
}
