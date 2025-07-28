// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Projectile_Boss.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "IntegritySystem/DamageTypeEnum.h"
#include "IntegritySystem/Interfaces/IntegrityHolder.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AProjectile_Boss::AProjectile_Boss()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionObjectType(ECC_GameTraceChannel4);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOverlap);
	SphereCollision->CanCharacterStepUpOn = ECB_No;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	//ProjectileMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->ProjectileGravityScale = 0;
}

void AProjectile_Boss::BeginPlay()
{	
	Super::BeginPlay();
}

void AProjectile_Boss::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) && OtherActor->Implements<UIntegrityHolder>())
	{
		IIntegrityHolder* IntegrityInterface = Cast<IIntegrityHolder>(OtherActor);
		IntegrityInterface->Execute_Damage(OtherActor, Damage, this);
	}
}
