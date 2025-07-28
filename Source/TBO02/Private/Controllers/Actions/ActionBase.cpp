#include "Controllers/Actions/ActionBase.h"
#include "Controllers/GenericPlayerController.h"

void UActionBase::SetUp(AGenericPlayerController* controller)
{
	if (!controller)
	{
		UE_LOG(LogTemp, Log, TEXT("Controller missing!"));
		return;
	}

	if(!controller->GetEnhancedInputComponent())
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot find enhanced input component"));
		return;
	}

	// Set the controller
	playerController = controller;

	// Set world
	world = playerController->GetWorld();

	// Set the player
	Player = controller->GetCurrentCharacter();

	// Bind Actions
		playerController->GetEnhancedInputComponent()->BindAction(
		action,
		eventTrigger,
		this,
		&ThisClass::Perform
	);

}

void UActionBase::ShutDown()
{
	if(!playerController)
	{
		return;
	}	
}

void UActionBase::Perform(const FInputActionValue& val)
{
	if(!playerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller not found at %s action"), *GetNameSafe(this));
		return;
	}

	if(!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("Player not found at %s action"), *GetNameSafe(this));
		return;
	}

	if(bEnableDebugMessage)
		UE_LOG(LogTemp, Log, TEXT("Performed %s action"), *GetNameSafe(this));
}
