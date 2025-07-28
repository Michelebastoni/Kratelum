// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileDataAssets/OrbProjectileDataAsset.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectilePoolingSubsystem.h"
#include "Weapons/Projectiles/OrbProjectile.h"


void UOrbProjectileDataAsset::Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction,
                                               TArray<FHitResult>& hitResults, AWeapon* instigatorWeapon)
{
	if (!world || !IsValid(projectileClass)) return;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	//AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(projectileClass, muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation(), spawnParams);
	AProjectileBase* projectile = UProjectilePoolingSubsystem::GetInstance(world)->GetProjectileByType(projectileClass);
	
	direction.Normalize();
	if (projectile)
	{
		projectile->SetActorLocationAndRotation(muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation());
		UProjectileMovementComponent* movementComponent = projectile->GetProjectileMovementComponent();
		movementComponent->Velocity = direction * projectileSpeed;
		projectile->SetWeaponInstigator(instigatorWeapon);
		projectile->OnProjectileHit.AddUniqueDynamic(this, &ThisClass::CallWeaponApplyDamage);
		projectile->SetProjectileDataAsset(this);
		projectile->GetProjectileMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}



