// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileDataAssets/ConeProjectileDataAsset.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectilePoolingSubsystem.h"
#include "Weapons/Projectiles/ProjectileBase.h"

void UConeProjectileDataAsset::Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction, TArray<FHitResult>& hitResults, AWeapon*
                                                instigatorWeapon)
{
	if (!world || !IsValid(projectileClass)) return;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	float halfAngle = coneAngle * 0.5f;
	float angleStep = (bulletAmount > 1) ? (coneAngle / (bulletAmount - 1)) : 0.0f;
	
	for (int i = 0; i < bulletAmount; i++)
	{
		
		//AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(projectileClass, muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation(), spawnParams);
		AProjectileBase* projectile = UProjectilePoolingSubsystem::GetInstance(world)->GetProjectileByType(projectileClass);
		
		direction.Normalize();
		
		if (projectile)
		{
			projectile->SetActorLocationAndRotation(muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation());
			
			float angle = -halfAngle + i * angleStep;
			FVector bulletDirection = FRotator(0, angle, 0).RotateVector(direction.GetSafeNormal());
			
			UProjectileMovementComponent* movementComponent = projectile->GetProjectileMovementComponent();
			movementComponent->Velocity = bulletDirection * projectileSpeed;
			projectile->SetWeaponInstigator(instigatorWeapon);
			projectile->OnProjectileHit.AddUniqueDynamic(this, &ThisClass::CallWeaponApplyDamage);
			projectile->SetProjectileDataAsset(this);
			projectile->GetProjectileMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}
