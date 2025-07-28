#include "Weapons/Projectiles/ProjectileBase.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "PoolingSystem/Actors/Pool.h"
#include "Weapons/Weapon.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"

AProjectileBase::AProjectileBase()
{
	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("Projectile Movement Component"));
	projectileMovementComponent->ProjectileGravityScale = 0;

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = projectileMesh;
	projectileMesh->SetGenerateOverlapEvents(true);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	projectileMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::ProjectileOnTriggerEnter);
}

void AProjectileBase::ProjectileOnTriggerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (OtherActor->IsA(StaticClass())) return;

		//Check if self hit
		if (OtherActor == this) return;
		if (IsValid(weaponInstigator))
		{
			if (OtherActor == weaponInstigator) return;
			if (IsValid(weaponInstigator->weaponHolder))
			{
				if (weaponInstigator->weaponHolder->GetClass() == OtherActor->GetClass()) return;
				if (OtherActor == weaponInstigator->weaponHolder) return;
			}
		}
	}

	if (projectileDataAsset)
	{
		if (projectileDataAsset->penetrating)
		{
			if (OtherComp->GetCollisionResponseToChannel(ECC_GameTraceChannel4) == ECR_Block)
			{
				OnLifetimeEnded();
			}
		}
		else
		{
			OnLifetimeEnded();
		}
	}

	OnProjectileHit.Broadcast(OtherActor, SweepResult, weaponInstigator);
}

void AProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	projectileMesh->OnComponentBeginOverlap.RemoveDynamic(this, &AProjectileBase::ProjectileOnTriggerEnter);
}

void AProjectileBase::OnLifetimeEnded()
{
	ProjectilePool->ReturnPoolable(this);
	projectileMovementComponent->StopMovementImmediately();
}

bool AProjectileBase::GetIsInUse_Implementation()
{
	return bIsPooled;
}

void AProjectileBase::Initialize_Implementation(APool* Pool)
{
	IPoolable::Initialize_Implementation(Pool);
	SetActorTickEnabled(false);
	ProjectilePool = Pool;
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsPooled = false;
	projectileMovementComponent->SetComponentTickEnabled(false);
	SetActorHiddenInGame(true);
}

void AProjectileBase::OnObjectPooled_Implementation()
{
	IPoolable::OnObjectPooled_Implementation();

	SetActorTickEnabled(true);
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bIsPooled = true;
	SetActorHiddenInGame(false);
	projectileMovementComponent->SetComponentTickEnabled(true);
	UE_LOG(LogPool, Display, TEXT("[Projectile %s] required from pool!]"), *this->GetName());
}

void AProjectileBase::OnObjectReturnToPool_Implementation()
{
	IPoolable::OnObjectReturnToPool_Implementation();
	SetActorTickEnabled(false);
	UE_LOG(LogPool, Display, TEXT("[Projectile %s] returned to pool!]"), *this->GetName());
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(PoolingTimerHandle);
	bIsPooled = false;
	projectileMovementComponent->SetComponentTickEnabled(false);
	SetActorHiddenInGame(true);
}
