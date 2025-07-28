// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Minion.h"

#include "Boss/Boss.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/FunctionLibrary.h"

// Sets default values
AMinion::AMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	CartMesh = CreateDefaultSubobject<UStaticMeshComponent>("CartMesh");
	CartMesh->SetupAttachment(Root);

	MinionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MinionMesh"));
	MinionMesh->SetupAttachment(CartMesh);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(MinionMesh);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(CartMesh);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AMinion::HandleHitBoxOverlap);

	OnDestroyed.AddDynamic(this, &AMinion::HandleOnDestroyed);
}

// Called when the game starts or when spawned
void AMinion::BeginPlay()
{
	CurrentHP = StartingHP;
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	Boss = Cast<ABoss>(OutActors[0]);

	Minions = UFunctionLibrary::GetAllActorsOfClassCasted<AMinion>(GetWorld());

	Super::BeginPlay();
}

// Called every frame
void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMinion::HandleDamage(float Damage, AActor* Harmer)
{
	if (Harmer->IsA(BarrelClass))
	{
		bIsShieldUp = false;
		ShieldMesh->SetVisibility(false, false);
		Damage = StartingHP / 2;
	}

	if (bIsShieldUp)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFX_DamageShield, GetActorLocation());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFX_Damage, GetActorLocation());
		CurrentHP -= Damage;

		if (CurrentHP <= 0)
			Destroy();
	}

	UpdateLifeBar();
}

void AMinion::HandleHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UIntegrityHolder>() && OtherActor != this)
	{
		IIntegrityHolder* IntegrityInterface = Cast<IIntegrityHolder>(OtherActor);
		IntegrityInterface->Execute_Damage(OtherActor, CollisionDamage, this);
	}	
}

void AMinion::HandleOnDestroyed(AActor* DestroyedActor)
{
	// Increase all other minion HP
	for (AMinion* Minion : Minions)
	{
		if(Minion != this && IsValid(Minion))
			Minion->CurrentHP += StartingHP * HPIncreasePerc;
	}	
}
