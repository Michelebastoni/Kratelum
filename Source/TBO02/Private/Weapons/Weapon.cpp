// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "CharacterLogic/GenericCharacter.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "InteractionSystem/Interfaces/Interactable.h"
#include "InteractionSystem/Components/InteractionComponent.h"
#include "IntegritySystem/Integrity.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Weapons/MeleeComponent.h"
#include "Weapons/WeaponParametersDataAsset.h"


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = weaponMesh;
	muzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	muzzleLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetUpData();

	MeleeComponent = GetComponentByClass<UMeleeComponent>();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MuzzlePos = muzzleLocation->GetComponentLocation();
	MuzzleForward = muzzleLocation->GetForwardVector();
}

bool AWeapon::ShootStraightAhead(float randomDeviation)
{
	return ShootTowards(muzzleLocation->GetComponentLocation() + muzzleLocation->GetForwardVector(), randomDeviation);
}

bool AWeapon::ShootTowards(const FVector& targetLocation, float randomDeviation)
{
		
	FVector direction = targetLocation - muzzleLocation->GetComponentLocation();
	direction.Normalize();
	return Shoot(direction, randomDeviation);
}

bool AWeapon::Shoot(FVector direction, float randomDeviation)
{
	if (CanShoot() != CanShootResponses::CanShoot)
		return false;

	APlayerCharacter* Player = Cast<APlayerCharacter>(weaponHolder);
	if (Player)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), equippedProjectile->gunshot, GetActorLocation());
	}

	
	//Rotate direction by weapon error
	float randomAngle = FMath::RandRange(0.0f, randomDeviation);
	FVector rotationAxis = FMath::VRand();
	direction = direction.RotateAngleAxis(randomAngle, rotationAxis);

	//Remove projectiles
	projectilesInMag--;
	
	TArray<FHitResult> hitResults;
	equippedProjectile->Shoot(GetWorld(), muzzleLocation, direction, hitResults, this);
		
	// for (int i = 0; i < hitActors.Num(); i++)
	// {
	// 	ApplyDamage(hitActors[i], hitResults[i], equippedProjectile);
	// 	if (AGenericCharacter* character = Cast<AGenericCharacter>(hitActors[i]))
	// 	{
	// 		UGameplayStatics::PlaySoundAtLocation(GetWorld(), equippedProjectile->impactSound, hitResults[i].ImpactPoint);
	// 		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), character->GetBloodVfx(), hitResults[i].ImpactPoint, hitResults[i].ImpactNormal.Rotation());
	// 	}
	// }
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), muzzleFlash, muzzleLocation->GetComponentLocation(), muzzleLocation->GetComponentRotation());
	//auto TrailVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), equippedProjectile->bulletFX, muzzleLocation->GetComponentLocation(), direction.Rotation());

	//TrailVFX->SetCustomTimeDilation(UAdrenalineSingletonSubsystem::AdrenalineSlowMult);

	lastShotTime = GetWorld()->GetTimeSeconds();

	onShoot.Broadcast(projectilesInMag);
	
	return true;
}

bool AWeapon::ApplyDamage(AActor* hitActor, FHitResult hitResult, UProjectileDataAsset* projectileDataAsset)
{
	if (hitActor)
	{
		//Apply physics impulse
		if (AGenericCharacter* character = Cast<AGenericCharacter>(hitActor))
		{
			FVector impact = hitResult.ImpactPoint - muzzleLocation->GetComponentLocation();
			impact.Normalize();
			character->ApplyImpact();
		}

		//Apply damage
		if (hitActor->Implements<UIntegrityHolder>())
		{
			IIntegrityHolder* integrityInterface = Cast<IIntegrityHolder>(hitActor);

			integrityInterface->Execute_Damage(hitActor, projectileDataAsset->damage, weaponHolder);
		}

		AGenericCharacter* character = Cast<AGenericCharacter>(weaponHolder);
		if (!character)
		{
			return false;
		}
		
		//Apply interaction
		if(hitActor->Implements<UInteractable>())
		{
			IInteractable* interactableActor = Cast<IInteractable>(hitActor);

			interactableActor->Execute_Interact(
				hitActor,
				character->interactionComponent,
				EInteractionMethod::Shooting);
		}
		return true;
	}
	return false;
}

void AWeapon::SetNewWeaponParameters(UWeaponParametersDataAsset* newParameters)
{
	if (newParameters->projectile)
	{
		equippedProjectile = newParameters->projectile;
	}

	if(newParameters->impactKillSound)
	{
		impactKillSound = newParameters->impactKillSound;
	}

	if(newParameters->muzzleFlash)
	{
		muzzleFlash = newParameters->muzzleFlash;
	}
	
	if(newParameters->weaponSprite)
	{
		weaponSprite = newParameters->weaponSprite;
	}

	if(newParameters->WeaponMesh)
	{
		weaponMesh->SetSkeletalMesh(newParameters->WeaponMesh);
	}

	if(newParameters->ShootingMontage)
	{
		shootAnimationMontage = newParameters->ShootingMontage;
	}

	if(newParameters->ReloadMontage)
	{
		reloadAnimationMontage = newParameters->ReloadMontage;
	}

	magSize = newParameters->magSize;
	shootingCooldown = newParameters->shootingCooldown;
	overflowValue = newParameters->overflowValue;
	lowAmmoThreshold = newParameters->lowAmmoThreshold;
	projectilesInMag = newParameters->magSize;

	// Set new weapon data
	WeaponData = newParameters;

}

void AWeapon::SetUpData()
{
	if(!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon data is not set properly into %s"), *GetNameSafe(this));
		return;
	}

	SetNewWeaponParameters(WeaponData);
}

bool AWeapon::StartReload(UProjectileDataAsset* newProjectile)
{
	if (CanReload() == CanReloadResponses::CanReload)
	{
		isReloading = true;
		//TODO Apply reload animation
		return true;
	}
	return false;
}

void AWeapon::ApplyReload()
{
	projectilesInMag = magSize;
	
	
	// if (projectileToReload != nullptr && projectileToReload != equippedProjectile)
	// {
	// 	ReplaceProjectile(projectileToReload);
	// }
	
}

void AWeapon::FinishReload()
{
	isReloading = false;

}

CanReloadResponses AWeapon::CanReload()
{
	if (isReloading) return CanReloadResponses::AlreadyReloading;

	if (projectilesInMag > 0) return CanReloadResponses::StillHasBullets;
	
	return CanReloadResponses::CanReload;
}

CanShootResponses AWeapon::CanShoot()
{
	if (projectilesInMag < 1)
	{
		return CanShootResponses::HasNoAmmo;
	}

	if (!bCanShoot)
	{
		return CanShootResponses::HasNoAmmo;
	}
		
	if (equippedProjectile == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current equipped projectile for weapon [%s] is null"), *GetActorNameOrLabel());
		return CanShootResponses::HasNoProjectile;
	}

	if (isReloading)
	{
		return CanShootResponses::IsReloading;
	}

	if (GetWorld()->GetTimeSeconds() < lastShotTime + shootingCooldown)
	{
		return CanShootResponses::WeaponCooldown;
	}
	
	return CanShootResponses::CanShoot;
}
