// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/Boss.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Character.h"
#include "IntegritySystem/Interfaces/IntegrityHolder.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABoss::ABoss()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(Mesh);
	ShieldMesh->SetVisibility(false);
	ShieldMesh->SetCollisionObjectType(ECC_WorldStatic);
	ShieldMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ShieldMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	CurrentHP = StartingHP;

	// Spawn Niagara Components for minions laser attacks
	Minions = GetAllActorsOfClass<AMinion>(GetWorld());
	Minions.Sort([](AMinion& A, AMinion& B){return A.Number < B.Number;}); // Sorting Lambda Expressions

	Super::BeginPlay();
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	if (bIsMinionsLaserActive)
		MoveLasers();

	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABoss::Heal(const float Amount)
{
	CurrentHP += Amount;
	if (CurrentHP > StartingHP)
		CurrentHP = StartingHP;
}

void ABoss::HandleDamage(float Damage, AActor* Harmer)
{
	if (bIsShieldUp)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFX_DamageShield, GetActorLocation());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFX_Damage, GetActorLocation());
		CurrentHP -= Damage;

		if (CurrentHP <= 0)
		{
			DetachFromControllerPendingDestroy();
			Destroy();
			UE_LOG(LogTemp, Error, TEXT("Boss destroyed!"));
		}
			
	}
}

void ABoss::SpawnProjectile(FProjectileParameters ProjectileParams)
{
	AProjectile_Boss* Projectile = GetWorld()->SpawnActorDeferred<AProjectile_Boss>(
		ProjectileClass, GetActorTransform());

	// Apply config 
	Projectile->ProjectileMovementComponent->InitialSpeed = ProjectileParams.Speed;
	Projectile->ProjectileMovementComponent->MaxSpeed = ProjectileParams.Speed;
	Projectile->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Projectile->SetActorScale3D(FVector(ProjectileParams.Scale, ProjectileParams.Scale, ProjectileParams.Scale));
	Projectile->SetLifeSpan(ProjectileParams.LifeSpan);
	Projectile->ProjectileMovementComponent->bIsHomingProjectile = ProjectileParams.bFollowPlayer;
	if (ProjectileParams.bFollowPlayer)
	{
		AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		Projectile->ProjectileMovementComponent->HomingTargetComponent = Player->GetRootComponent();
		Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileParams.HomingStrength;
	}

	FTransform ProjectileTransform;
	ProjectileTransform.SetLocation(ProjectileParams.Location);
	ProjectileTransform.SetRotation(ProjectileParams.Rotation.Quaternion());

	Projectile->FinishSpawning(ProjectileTransform);
}

void ABoss::StopTimers(FTimerHandle TimerHandle, FTimerHandle StopHandle)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StopHandle); // Probably useless
}

void ABoss::SpawnProjectileTowardPlayer(FProjectileParameters& ProjectileParams, float AngleVariance,
                                        float SpeedVarianceMult)
{
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = Player->GetActorLocation();
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *Player->GetVelocity().ToString());
	ProjectileParams.Rotation = UKismetMathLibrary::FindLookAtRotation(ProjectileParams.Location, PlayerLocation);

	// Random variance
	ProjectileParams.Rotation.Yaw += FMath::FRandRange(-AngleVariance, AngleVariance);
	ProjectileParams.Speed *= 1.0f + FMath::FRandRange(-SpeedVarianceMult, SpeedVarianceMult);

	SpawnProjectile(ProjectileParams);
}

void ABoss::SpawnBurstTowardPlayer(float Duration, float SpawnDelay, float AngleVariance, float SpeedVarianceMult,
                                   const FProjectileParameters& ProjectileParams)
{
	FTimerHandle NewTimerHandle;
	FTimerHandle NewStopTimerHandle;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("SpawnProjectileTowardPlayer"), ProjectileParams, AngleVariance,
	                            SpeedVarianceMult);
	GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, TimerDelegate, SpawnDelay, true);

	FTimerDelegate StopTimerDelegate;
	StopTimerDelegate.BindUFunction(this, FName("StopTimers"), NewTimerHandle, NewStopTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(NewStopTimerHandle, StopTimerDelegate, Duration, false);
}

// Func to spawn a wave of projectiles that rotates 
void ABoss::SpawnProjectiles(const int SpawnPointCount, const float SpawnRadius, const float AngleOffsetRad,
                             FProjectileParameters ProjectileParameters)
{
	if (!ProjectileClass)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, // Key (-1 means "new message every time")
			5.0f, // Duration (in seconds)
			FColor::Yellow, // Text color
			TEXT("Boss Project class not valid!") // Your message
		);
		return;
	}

	float StepAngle = 2 * PI / SpawnPointCount;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int i = 0; i < SpawnPointCount; i++)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.X += SpawnRadius * FMath::Cos(StepAngle * i + AngleOffsetRad);
		SpawnLocation.Y += SpawnRadius * FMath::Sin(StepAngle * i) + AngleOffsetRad;
		SpawnLocation.Z += 50;

		FRotator SpawnRotation = FRotator(0.0f, FMath::RadiansToDegrees(StepAngle * i + AngleOffsetRad), 0.0f);

		ProjectileParameters.Rotation = SpawnRotation;
		SpawnProjectile(ProjectileParameters);
	}
}

void ABoss::MinionsLaserAttack(float LaserSpeed)
{
	bIsMinionsLaserActive = true;

	// Create lasers for minion pairs along a square
	for (int i = 0; i < Minions.Num(); i++)
	{
		if (IsValid(Minions[i]))
		{
			FVector Location1 = Minions[i]->GetActorLocation();
			FVector Location2;
			if (i == Minions.Num() - 1 && IsValid(Minions[0]))
				Location2 = Minions[0]->GetActorLocation();
			else if (IsValid(Minions[i + 1]))
				Location2 = Minions[i + 1]->GetActorLocation();
			else
				continue;

			UNiagaraComponent* NewNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), // The world context
				LaserNiagaraSystem, // The Niagara system to spawn
				Location1, // Location to spawn at
				FRotator(0, 0, 0), // Rotation of the effect
				FVector(1, 1, 1), // Scale of the effect
				false, // Auto-destroy after completion
				true, // Auto-activate
				ENCPoolMethod::AutoRelease // Pooling method
			);

			NewNiagara->SetVectorParameter(FName("BeamEnd"), Location2);

			// Start Laser anticipation (at the end start movement)
			FTimerHandle NewTimerHandle;

			FLaserData LaserData {
				NewNiagara,
				Location1,
				Location2,
				LaserSpeed,
				false,
				NewTimerHandle
			};
			LasersDatas.Add(NewNiagara, LaserData);

			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, FName("ToggleMinionLaserVisibility"), NewNiagara, 0.5f);
			GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, TimerDelegate, 0.5f, false);
		}
	}
}

void ABoss::SetShieldActive(bool bActive)
{
	bIsShieldUp = bActive;
	ShieldMesh->SetVisibility(bActive);
}

void ABoss::ToggleMinionLaserVisibility(UNiagaraComponent* NiagaraComponent, float ElapsedTime)
{
	FLaserData* LaserData = LasersDatas.Find(NiagaraComponent);
	if (!LaserData)
	{
		UE_LOG(LogTemp, Error, TEXT("Minion LaserData not found!"));
		return;
	}

	float TimerRate = 0.5;
	bool IsVisible = LaserData->NiagaraComponent->GetVisibleFlag();

	if (IsVisible)
	{
		LaserData->NiagaraComponent->SetVisibility(false);
		TimerRate = 0.2f;
	}
	else
	{
		LaserData->NiagaraComponent->SetVisibility(true);
		TimerRate = 0.5f;
	}

	ElapsedTime += TimerRate;
	if (ElapsedTime > MinionLaserAnticipationDuration)
	{
		// Stop the anticipation and start moving the laser 
		LaserData->NiagaraComponent->SetVisibility(true);
		LaserData->bIsMoving = true;
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(LaserData->TimerHandle);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ToggleMinionLaserVisibility"), NiagaraComponent, ElapsedTime);
	GetWorld()->GetTimerManager().SetTimer(LaserData->TimerHandle, TimerDelegate, TimerRate, false);
}

void ABoss::MoveLasers()
{
	for (TMap<UNiagaraComponent*, FLaserData>::TIterator It = LasersDatas.CreateIterator(); It; ++It)
	{
		// Move laser
		FLaserData& LaserDataRef = It.Value();
		FVector& Location1 = LaserDataRef.Location1;
		FVector& Location2 = LaserDataRef.Location2;
		if (LaserDataRef.bIsMoving)
		{
			float MovSpeed = LaserDataRef.LaserSpeed * GetWorld()->GetDeltaSeconds();
			Location1 += (GetActorLocation() - Location1).GetSafeNormal() * MovSpeed;
			Location2 += (GetActorLocation() - Location2).GetSafeNormal() * MovSpeed;
			LaserDataRef.NiagaraComponent->SetWorldLocation(Location1);
			LaserDataRef.NiagaraComponent->SetVectorParameter(FName("BeamEnd"), Location2);
		}

		// Check collisions
		TArray<FHitResult> HitResults;
		FVector Start = Location1;
		FVector End = Location2; // Trace 1000 units forward
		float Radius = 10.0f;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			Start,
			End,
			FQuat::Identity, // No rotation
			ECC_Pawn,
			FCollisionShape::MakeSphere(Radius), // Define the sphere size
			CollisionParams
		);
		if (bHit)
		{
			UClass* PlayerClass = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetClass();
			for (FHitResult HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor->IsA(PlayerClass))
				{
					IIntegrityHolder* IntegrityInterface = Cast<IIntegrityHolder>(HitActor);
					IntegrityInterface->Execute_Damage(HitActor, MinionLaserDamage, this);
				}
			}
		}

		// Destroy if reached Boss
		if ((Location1 - GetActorLocation()).Length() <= 100.0f)
		{
			LaserDataRef.NiagaraComponent->DestroyComponent();
			LasersDatas.Remove(It->Key);
		}
	}
}
