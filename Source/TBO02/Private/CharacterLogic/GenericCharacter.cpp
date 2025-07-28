#include "CharacterLogic/GenericCharacter.h"

#include "IntegritySystem/Integrity.h"
#include "InteractionSystem/Components/InteractionComponent.h"

#include "Weapons/Weapon.h"
#include "Weapons/MeleeComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraFunctionLibrary.h"
#include "CharacterLogic/PlayerCharacter.h"

#include "InteractionSystem/Components/InteractionComponent.h"

#include "Controllers/GenericPlayerController.h"
#include "Widgets/Widget_Reload/QTEReload.h"


AGenericCharacter::AGenericCharacter() : bCanMove(true), bCanRotate(true), bIsAffectedByReloadMultiplier(true),
                                         bIsAlive(true), bIsAwaitingFreeze(false)
{
	PrimaryActorTick.bCanEverTick = false;

	// Add intergrity component
	integrityComponent = CreateDefaultSubobject<UIntegrity>(TEXT("Intergrity Component"));

	// -------------- TO REWORK ------------------------
	interactionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));
	// --------------------------------------------------

	MeshPtr = GetMesh();
	CapsulePtr = GetCapsuleComponent();

	MeshPtr->SetCollisionProfileName("NoCollision");
}

void AGenericCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn and set weapon
	if (startWithWeapon)
	{
		SpawnWeapon();
	}

	//Prepare collisonParam to damageAOE logic
	CharacterParam.bTraceComplex = false;
	CharacterParam.AddIgnoredActor(this);
	if (currentWeapon)
	{
		CharacterParam.AddIgnoredActor(CastChecked<AActor>(currentWeapon.Get()));
	}

	// Assign functions into intergrity delegates
	integrityComponent->OnDeath.AddDynamic(this, &ThisClass::HandleCharacterDeath);
	integrityComponent->OnRemoveIntegrity.AddDynamic(this, &ThisClass::HandleOnChangeIntegrity);
}

void AGenericCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (integrityComponent)
	{
		integrityComponent->OnDeath.RemoveDynamic(this, &ThisClass::HandleCharacterDeath);
		integrityComponent->OnChangeIntegrity.RemoveDynamic(this, &ThisClass::HandleOnChangeIntegrity);
	}
}

void AGenericCharacter::SpawnWeapon()
{
	if (!IsValid(weaponClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon class is invalid!"));
		return;
	}

	// Spawn Weapon
	AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(weaponClass);

	// Set weapon
	SetCurrentWeapon(weapon);

	// Set melee
	SetCurrentMeleeHolder(weapon);
}

void AGenericCharacter::ChangeWeaponParameters(UWeaponParametersDataAsset* newWeaponAsset, float weaponDuration,
                                               bool bIsWeaponBase)
{
	if (!currentWeapon)
	{
		return;
	}

	checkf(newWeaponAsset, TEXT("Weapon Asset is NULL, Source: [%s]"), *GetName());

	currentWeapon->SetNewWeaponParameters(newWeaponAsset);

	// Trigger event
	OnChangeWeapon.Broadcast(currentWeapon, weaponDuration, bIsWeaponBase);
}

/// <summary>
/// This method indicate if character can see the target passed by second parameter troughth line trace
/// </summary>
/// <param name="startPoint"> the point where line trace should start</param>
/// <param name="target"> the point where line trace should end </param>
/// <returns> if line trace hit something return false, otherwise true </returns>
bool AGenericCharacter::CanSee(FVector startPoint, FVector target)
{
	FHitResult hit;
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;

	GetWorld()->LineTraceSingleByChannel(
		hit,
		startPoint,
		target,
		ECC_Visibility,
		params
	);

	//DrawDebugLine(GetWorld(), startPoint, target, FColor::Red);

	if (hit.bBlockingHit)
	{
		// DEBUG MESSAGE
		if (GEngine && showMessage)
		{
			GEngine->AddOnScreenDebugMessage(
				6,
				2.0f,
				FColor::Orange,
				FString::Printf(
					TEXT("%s can't see the target due line trace hit: %s"),
					*GetActorNameOrLabel(),
					*GetNameSafe(hit.GetActor())
				)
			);
		}

		return false;
	}

	return true;
}

void AGenericCharacter::Damage_Implementation(float damage, AActor* harmer)
{
	integrityComponent->SubtractIntegrityValue(damage, harmer);
}

void AGenericCharacter::Restore_Implementation(float restoreValue, AActor* harmer)
{
	integrityComponent->AddIntegrityValue(restoreValue, harmer);
}


/// <summary>
/// This method must stop physics simulation and also disable tick 
/// </summary>
void AGenericCharacter::FreezeCharacter()
{
	if (!MeshPtr || !MeshPtr->GetAnimInstance())
	{
		return;
	}

	MeshPtr->GetAnimInstance()->SavePoseSnapshot(deathPoseNameSnapshot);
	MeshPtr->SetSimulatePhysics(false);

	GetWorldTimerManager().ClearTimer(RagdollTimer);
	GetWorldTimerManager().ClearTimer(freezeTimer);
}

void AGenericCharacter::UnfreezeCharacter()
{
	if (!MeshPtr)
	{
		return;
	}

	if (const auto AnimInstance = MeshPtr->GetAnimInstance())
	{
		// Remove death snapshot
		if (AnimInstance->GetPoseSnapshot(deathPoseNameSnapshot))
			AnimInstance->RemovePoseSnapshot(deathPoseNameSnapshot);

		// Reset animation
		AnimInstance->InitializeAnimation();
	}

	MeshPtr->AttachToComponent(CapsulePtr, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MeshPtr->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
}

void AGenericCharacter::SetCurrentWeapon(AWeapon* newWeapon, const float weaponDuration, bool bIsWeaponBase)
{
	if (!newWeapon)
	{
		return;
	}

	currentWeapon = newWeapon;

	currentWeapon->weaponHolder = this;

	currentWeapon->ApplyReload();

	FAttachmentTransformRules transformRules{EAttachmentRule::KeepRelative, true};

	// Attach to socket
	currentWeapon->AttachToComponent(
		MeshPtr,
		transformRules,
		weaponSocketName
	);

	// Trigger event
	OnChangeWeapon.Broadcast(currentWeapon, weaponDuration, bIsWeaponBase);
}

void AGenericCharacter::SetCurrentMeleeHolder(AWeapon* meleeWeapon)
{
	if (!meleeWeapon)
	{
		return;
	}

	currentMelee = meleeWeapon->GetMeleeComponent();
	if (!currentMelee)
	{
		return;
	}

	currentMelee->SetMeleeHolder(this);
}

void AGenericCharacter::SetMaxSpeed(float newMaxSpeed)
{
	maxSpeed = newMaxSpeed;
	GetCharacterMovement()->MaxWalkSpeed = maxSpeed;
}

/// <summary>
/// This method must apply impact when the character gets hit
/// </summary>
void AGenericCharacter::ApplyImpact()
{
	if (bIsAlive || bIsAwaitingFreeze)
	{
		if (reactionMontage)
		{
			// Prepera Data
			int32 montageSections = reactionMontage->GetNumSections();
			int32 randomSection = FMath::FRandRange(0.0f, montageSections);
			float sectionStartTime = 0.0f;
			float sectionEndTime = 0.0f;

			// Get random montage section
			reactionMontage->GetSectionStartAndEndTime(randomSection, sectionStartTime, sectionEndTime);

			// Play montage
			MeshPtr->GetAnimInstance()->Montage_Play(
				reactionMontage,
				1.0f,
				EMontagePlayReturnType::MontageLength,
				sectionStartTime,
				false
			);
		}
	}
}

void AGenericCharacter::SetRagdoll() const
{
	MeshPtr->SetCollisionProfileName("Ragdoll");

	MeshPtr->SetSimulatePhysics(true);
	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGenericCharacter::UnsetRagdoll() const
{
	MeshPtr->SetSimulatePhysics(false);
	MeshPtr->SetCollisionProfileName("NoCollision");
	CapsulePtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool AGenericCharacter::MakeAOEDamage(const float radius, const float damage,
                                      const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
                                      bool drawDebug,
                                      USoundBase* explositonSound, UNiagaraSystem* damageVFX)
{
	const FVector Pos = GetActorLocation();

	// Prepare data
	TArray<FHitResult> OutHits;
	const FCollisionObjectQueryParams ObjectParams = SetCollisionObjectParams(ObjectTypes);

	if (!ObjectParams.IsValid())
	{
		UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Invalid object types"));
		return false;
	}

	// Make multi sphere trace
	GetWorld()->SweepMultiByObjectType(
		OutHits,
		Pos,
		Pos,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(radius),
		CharacterParam
	);

	// Play explosion sound
	if (explositonSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), explositonSound);
	}

	// Play explosion vfx
	if (damageVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), damageVFX, Pos, FRotator::ZeroRotator,
		                                               FVector::OneVector, false, true, ENCPoolMethod::AutoRelease);
	}

	// DEBUG MESSAGE
	if (drawDebug)
		DrawDebugSphere(
			GetWorld(),
			Pos,
			radius,
			10,
			FColor::Red,
			false,
			2.0f
		);

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
					interfaceActor->Execute_Damage(hitActor, damage, this);

				if (hitActor->IsA(APlayerCharacter::StaticClass()))
				{
					bHasHitPlayer = true;
				}
			}
		}
	}
	return bHasHitPlayer;
}

FCollisionObjectQueryParams AGenericCharacter::SetCollisionObjectParams(
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

void AGenericCharacter::ChangeWeaponProjectile(UProjectileDataAsset* newProjectile)
{
	currentWeapon->EquipProjectile(newProjectile);
}

/// <summary>
/// Method triggered by character death (when intergrity "On dead" event trigger)
/// this method indicate the character's behavior when he dies
/// </summary>
/// <param name="character"> ref to the character itself </param>
/// <param name="component"> ref to intergrity component </param>
/// <param name="CurrentIntegrityValue">ref to the current integrity </param>
void AGenericCharacter::HandleCharacterDeath(const AActor* character, const AActor* harmer,
                                             const class UIntegrity* component,
                                             const float currentIntegrity)
{
	// DEBUG MESSAGE
	if (GEngine && showMessage)
	{
		GEngine->AddOnScreenDebugMessage(
			10,
			1.0f,
			FColor::Magenta,
			FString::Printf(TEXT("%s is dead!"), *GetNameSafe(character))
		);
	}

	// Get skeletal mesh and capsule component and set collision channel and response
	SetRagdoll();

	GetWorldTimerManager().SetTimer(
		RagdollTimer,
		this,
		&ThisClass::FreezeCharacter,
		3.0f);

	// Off the outline mat
	if (outlineMatInstance)
	{
		outlineMatInstance->SetScalarParameterValue("Outline", 0.0f);
	}

	// Update character status that is ready to freeze (stop physics simulation)
	bIsAwaitingFreeze = true;

	// Update character status, useful to handle if character, after being hit, has to recover itself
	bIsAlive = false;

	// Trigger event
	OnCharacterDeath.Broadcast(this, currentWeapon, harmer, integrityComponent);
}

/// <summary>
/// Method triggered by character hit (when integrity "on hit" event trigger)
/// this method indicate the character's behavior when he being hit by something/someone
/// </summary>
/// <param name="character"> ref to the character it self </param>
/// <param name="component"> ref to intergrity component </param>
/// <param name="currentDamage"> ref to the current damage taken </param>
void AGenericCharacter::HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue,
                                                const float currentIntegrityValue, const float changedValue,
                                                const float integrityValueRate, AActor* invader)
{
	if (!bIsAlive)
	{
		return;
	}

	// DEBUG MESSAGE
	if (GEngine && showMessage)
	{
		GEngine->AddOnScreenDebugMessage(
			11,
			1.0f,
			FColor::Orange,
			FString::Printf(TEXT("%s has taken %.2f damage, and now has %.2f hp!"),
			                *GetNameSafe(this),
			                changedValue,
			                integrityComponent->GetCurrentIntegrityValue())
		);
	}

	ApplyImpact();

	OnCharacterHit.Broadcast();
}
