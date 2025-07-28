#include "Controllers/Actions/MovementAction.h"
#include "Controllers/GenericPlayerController.h"

#include "CharacterLogic/PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"


void UMovementAction::SetUp(AGenericPlayerController* controller)
{
	Super::SetUp(controller);
	
	// Set character initial speed
	SetNewSpeed(walkSpeed);

}

void UMovementAction::SetNewSpeed(const float NewSpeed)
{
	MovementData.WalkSpeed = NewSpeed;

	if (Player)
	{
		Player->SetMaxSpeed(MovementData.WalkSpeed);
	}
}

void UMovementAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if (!Player->CanCharacterMove())
	{
		// DEBUG MESSAGE
		UE_LOG(LogTemp, Display, TEXT("Character cannot move!"));
		return;
	}

	// Calculate speed value
	float speedValue =
		Player->GetMaxSpeed()					// Base speed
		* Player->GetMovementSpeedMultiplier(); // Speed multiplier

	// Read input data
	const FVector2D input = val.Get<FVector2D>();

	// Prepare Data
	FVector rightDirection = playerController->PlayerCameraManager->GetCameraRotation().Quaternion().GetRightVector();
	FVector ForwardDirection;

	ForwardDirection = playerController->PlayerCameraManager->GetCameraRotation().Quaternion().GetForwardVector();
	

	// Apply movement settings
	FVector2D newInput = input * speedValue;
	

	// Multiply delta time
	if(world)
	{
		newInput *= world->GetDeltaSeconds();
	}

	// Apply movement
	Player->AddMovementInput(ForwardDirection, newInput.X);
	Player->AddMovementInput(rightDirection, newInput.Y);
}
