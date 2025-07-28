#include "Controllers/Actions/ShootAction.h"
#include "Controllers/GenericPlayerController.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Weapons/Weapon.h"

void UShootAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if (!Player->GetCurrentWeapon())
	{
		// DEBUG MESSAGE
		UE_LOG(LogTemp, Error, TEXT("Weapon Missing!"));
		return;
	}

	if (Player->GetCurrentWeapon()->IsReloading())
	{
		return;
	}

	// If cant shoot due character is reloading or doing melee attack return
	if (!Player->GetCurrentWeapon()->Shoot(Player->GetActorForwardVector()))
	{
		return;
	}

	// Trigger shoot animation
	if (Player->GetCurrentWeapon()->GetCharacterShootAnimationMontage())
	{
		Player->GetMesh()->GetAnimInstance()->Montage_Play(
			Player->GetCurrentWeapon()->GetCharacterShootAnimationMontage(),
			1.0f,
			EMontagePlayReturnType::MontageLength,
			0.0f,
			false
		);
	}
}
