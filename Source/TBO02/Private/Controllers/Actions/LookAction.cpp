#include "Controllers/Actions/LookAction.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Camera/CameraComponent.h"


void ULookAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if (!Player->CanCharacterRotate())
	{
		UE_LOG(LogTemp, Display, TEXT("%s cannot rotate"), *GetNameSafe(Player));
		return;
	}

	FVector  cameraForward = Player->GetCameraComponent()->GetForwardVector();
	cameraForward.Z = 0.0f;
	cameraForward.Normalize();

	FVector  cameraRight = Player->GetCameraComponent()->GetRightVector();
	FVector2D input = val.Get<FVector2D>();

	if(FMath::IsNearlyZero(input.X) && FMath::IsNearlyZero(input.Y))
	{
		return;
	}


	cameraRight.Z = 0.0f;

	input.Normalize();

	FVector DesiredDirection = (cameraForward * input.Y) + (cameraRight * input.X);
	
	Player->RotateCharacter(DesiredDirection);
	
}
