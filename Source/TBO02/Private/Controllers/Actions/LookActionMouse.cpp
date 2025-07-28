#include "Controllers/Actions/LookActionMouse.h"

#include "Camera/CameraComponent.h"
#include "Controllers/GenericPlayerController.h"
#include "CharacterLogic/PlayerCharacter.h"

void ULookActionMouse::Perform(const FInputActionValue& val)
{	
	Super::Perform(val);

	FVector InputVector = val.Get<FVector>();
	InputVector.X *= -1;
	
	if (InputVector.IsNearlyZero())
		return;
	
	FRotator CameraRotation = Player->GetCameraComponent()->GetComponentRotation();
	CameraRotation.Pitch = 0;
	CameraRotation.Roll = 0;
	CameraRotation.Normalize();

	FRotator InputRotation = InputVector.Rotation();
	
	FQuat InputQuat = InputRotation.Quaternion();
	FQuat CameraQuat = CameraRotation.Quaternion();

	FQuat TargetQuat = CameraQuat.Inverse() * InputQuat;
	
	FRotator TargetRotation = TargetQuat.Rotator();
	
	FRotator CurrentRotation = Player->GetActorRotation();
	float RotationSpeed = InputVector.Length() * RotationSensitivity;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, world->GetDeltaSeconds(), RotationSpeed);
	
	Player->SetActorRotation(NewRotation);
}
