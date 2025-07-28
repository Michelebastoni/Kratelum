#include "Weapons/ProjectileDataAssets/RocketProjectileDataAsset.h"

#include "NiagaraFunctionLibrary.h"
#include "CharacterLogic/GenericCharacter.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/ProjectilePoolingSubsystem.h"
#include "Weapons/Weapon.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "Weapons/Projectiles/RocketProjectile.h"

void URocketProjectileDataAsset::Shoot(UWorld* world, USceneComponent* muzzleLocation, FVector direction,
										TArray<FHitResult>& hitResults, class AWeapon* instigatorWeapon)
{
	if (!world || !IsValid(projectileClass)) return;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// world->SpawnActor<AProjectileBase>(projectileClass, muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation(), spawnParams);
	projectile = Cast<ARocketProjectile>(UProjectilePoolingSubsystem::GetInstance(world)->GetProjectileByType(projectileClass));
	World = world;
	direction.Normalize();
	if (projectile)
	{
		projectile->SetActorLocationAndRotation(muzzleLocation->GetComponentLocation(),
												muzzleLocation->GetComponentRotation());
		UProjectileMovementComponent* movementComponent = projectile->GetProjectileMovementComponent();
		movementComponent->Velocity = direction * projectileSpeed;
		projectile->SetWeaponInstigator(instigatorWeapon);
		projectile->OnProjectileHit.AddUniqueDynamic(this, &ThisClass::Explode);
		projectile->SetProjectileDataAsset(this);
		projectile->GetProjectileMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void URocketProjectileDataAsset::Explode(AActor* actor, FHitResult hitResult, AWeapon* weapon)
{
	if (!IsValid(weapon)) return;

	// Prepare data
	TArray<FHitResult> OutHits;
	const FCollisionObjectQueryParams ObjectParams = SetCollisionObjectParams(LayersToHit);

	FCollisionQueryParams ProjectileParam;
	ProjectileParam.bTraceComplex = false;
	
	if (!ObjectParams.IsValid())
	{
		UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Invalid object types"));
		return;
	}

	// Make multi sphere trace
	World->SweepMultiByObjectType(
		OutHits,
		hitResult.ImpactPoint,
		hitResult.ImpactPoint,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(ExplosionRadius),
		ProjectileParam
	);

	// Play explosion sound
	if (impactSound)
	{
		UGameplayStatics::PlaySound2D(World, impactSound);
	}

	// Play explosion vfx
	if (ExplosionVfx)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ExplosionVfx, hitResult.ImpactPoint, FRotator::ZeroRotator,
													   FVector::OneVector, false, true, ENCPoolMethod::AutoRelease);
	}

	bool bHasHitPlayer = false;
	// Make damage
	for (FHitResult& hit : OutHits)
	{
		if (AActor* hitActor = hit.GetActor())
		{
			UE_LOG(LogTemp, Display, TEXT("AOE Dash hit: %s"), *GetNameSafe(hitActor));

			if (hitActor->Implements<UIntegrityHolder>())
			{
				IIntegrityHolder* interfaceActor = Cast<IIntegrityHolder>(hitActor);
				if (interfaceActor)
					interfaceActor->Execute_Damage(hitActor, damage, weapon->weaponHolder);

				if (hitActor->IsA(APlayerCharacter::StaticClass()))
				{
					bHasHitPlayer = true;
				}
			}
		}
	}

	projectile->SetHasHitPlayer(bHasHitPlayer);
}

FCollisionObjectQueryParams URocketProjectileDataAsset::SetCollisionObjectParams(
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes)
{
	TArray<TEnumAsByte<ECollisionChannel>> CollisionObjectTraces;
	CollisionObjectTraces.AddUninitialized(ObjectTypes.Num());

	for (auto Iter = ObjectTypes.CreateConstIterator(); Iter; ++Iter)
	{
		CollisionObjectTraces[Iter.GetIndex()] = UEngineTypes::ConvertToCollisionChannel(*Iter);
	}

	FCollisionObjectQueryParams ObjectParams;
	for (auto Iter = CollisionObjectTraces.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel& Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			ObjectParams.AddObjectTypesToQuery(Channel);
		}
		else
		{
			UE_LOG(LogBlueprintUserMessages, Warning, TEXT("%d isn't valid object type"), (int32)Channel);
		}
	}

	return ObjectParams;
}
