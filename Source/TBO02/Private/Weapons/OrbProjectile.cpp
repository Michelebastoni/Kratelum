// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/OrbProjectile.h"
#include "Weapons/Weapon.h"

// Sets default values
AOrbProjectile::AOrbProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//projectileMesh->SetCollisionProfileName("Trigger");
	
	orbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Orb Mesh"));
	orbMesh->SetupAttachment(RootComponent);
	orbMesh->SetCollisionProfileName("Trigger");
	orbMesh->SetGenerateOverlapEvents(true);
	//orbMesh->RegisterComponent();
	//AddInstanceComponent(orbMesh);
}

// Called when the game starts or when spawned
void AOrbProjectile::BeginPlay()
{
	Super::BeginPlay();
	orbMesh->OnComponentBeginOverlap.AddDynamic(this, &AOrbProjectile::OrbOnTriggerEnter);
	
}

void AOrbProjectile::OrbOnTriggerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;
	if (OtherActor->IsA(StaticClass())) return;

	//Check if self hit
	if (IsValid(weaponInstigator))
	{
		if (OtherActor == weaponInstigator) return;
		if (IsValid(weaponInstigator->weaponHolder))
			if (OtherActor == weaponInstigator->weaponHolder) return;
	}
	

	//TODO Replace with modifiers
	OnProjectileHit.Broadcast(OtherActor, SweepResult, weaponInstigator);
}
