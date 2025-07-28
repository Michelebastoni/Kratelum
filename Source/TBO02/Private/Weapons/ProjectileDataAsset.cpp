// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"

#include "Weapons/Weapon.h"
#include "Weapons/Projectiles/ProjectileBase.h"

void UProjectileDataAsset::CallWeaponApplyDamage(AActor* actor, FHitResult hitResult, AWeapon* weapon)
{
	if (!IsValid(weapon)) return;
	
	weapon->ApplyDamage(actor, hitResult, this);
}
