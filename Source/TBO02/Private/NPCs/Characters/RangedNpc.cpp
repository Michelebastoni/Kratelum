#include "NPCs/Characters/RangedNpc.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "Weapons/Weapon.h"

void ARangedNpc::Attack(AActor* Target)
{
	if (!currentWeapon) return;

	ANpcControllerBase* controller = Cast<ANpcControllerBase>(GetController());
	APlayerCharacter* target = controller->GetCurrentPlayerTarget();
	
	if (!target) return;
	
	currentWeapon->ShootTowards(target->GetMesh()->GetSocketLocation(target->chestSocketName), weaponRandomDeviation);
	PlayAnimMontage(currentWeapon->GetCharacterShootAnimationMontage());
}

void ARangedNpc::Reload()
{
	if (!currentWeapon) return;
	if (currentWeapon->CanReload() == CanReloadResponses::CanReload)
	{
		currentWeapon->StartReload();
		PlayAnimMontage(currentWeapon->GetCharacterReloadAnimationMontage());
	}

}

bool ARangedNpc::CanReload()
{
	if (!currentWeapon) return false;
	return currentWeapon->CanReload() == CanReloadResponses::CanReload;
}
