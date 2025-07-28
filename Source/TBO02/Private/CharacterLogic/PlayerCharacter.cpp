#include "CharacterLogic/PlayerCharacter.h"
#include "OverflowSystem/Overflow.h"
#include "Camera/CameraComponent.h"
#include "IntegritySystem/Integrity.h"
#include "GameFramework/SpringArmComponent.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "Components/DecalComponent.h"
#include "Weapons/Weapon.h"
#include "Controllers/GenericPlayerController.h"
#include "Controllers/Actions/DashAction.h"

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(World);

	if (!NPCSubsystem)
	{
		return;
	}

	NPCSubsystem->SetPlayer(this);

	if (IsValid(MagazineRingMat))
	{
		MagazineRing = UMaterialInstanceDynamic::Create(MagazineRingMat, this, "ReloadSliderMat");
		MagazineRingComponent->SetDecalMaterial(MagazineRing);
		MagazineRing->SetScalarParameterValue(MagazineRingPercentName, 1.0f);
	}

	if(currentWeapon)
	{
		currentWeapon->onShoot.AddDynamic(this, &ThisClass::HandleMagazineDecal);
	}

}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	currentWeapon->onShoot.RemoveDynamic(this, &ThisClass::HandleMagazineDecal);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Player_Controller = Cast<AGenericPlayerController>(NewController);

	Super::PossessedBy(NewController);
}

APlayerCharacter::APlayerCharacter()
{
	// Add components
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	overflowComponent = CreateDefaultSubobject<UOverflow>(TEXT("Overflow Component"));
	MagazineRingComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("MagazineRingDecal"));

	MagazineRingComponent->SetupAttachment(RootComponent);

	// Set default spring arm values
	springArm->bInheritPitch = false;
	springArm->bInheritYaw = false;
	springArm->bUsePawnControlRotation = true;
	springArm->SetupAttachment(RootComponent);

	// Set camera settings
	camera->SetupAttachment(springArm);
}

void APlayerCharacter::HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue,
                                               const float currentIntegrityValue, const float changedValue,
                                               const float integrityValueRate, AActor* invader)
{
	Super::HandleOnChangeIntegrity(integrityComp, previousIntegrityValue, currentIntegrityValue, changedValue,
	                               integrityValueRate, invader);

	if (invicibilityTimer.IsValid())
	{
		return;
	}

	// Make character invicible
	integrityComponent->AddImmortalitySource();

	GetWorldTimerManager().SetTimer(
		invicibilityTimer,
		this,
		&ThisClass::OnInvincibilityEnd,
		invicibilityTime,
		false
	);

	OnStartInvicibility.Broadcast(this);
}

void APlayerCharacter::ChangeWeaponParameters(UWeaponParametersDataAsset* newWeaponAsset, float weaponDuration,
                                              bool bIsWeaponBase)
{
	Super::ChangeWeaponParameters(newWeaponAsset, weaponDuration, bIsWeaponBase);

	HandleMagazineDecal(currentWeapon->GetAmmoAmount());
}


void APlayerCharacter::RotateCharacter(const FVector& deltaRotation)
{
	const FVector normalizeVector = deltaRotation.GetSafeNormal();

	const FRotator desireRotation = FRotationMatrix::MakeFromXZ(normalizeVector, FVector::UpVector).Rotator();
	const FRotator newRotation = FRotator{0.0f, desireRotation.Yaw, 0.0f};

	// Set character rotation
	SetActorRotation(newRotation);
}

void APlayerCharacter::OnInvincibilityEnd()
{
	// Player Can recive damage
 	integrityComponent->RemoveImmortalitySource();

	GetWorldTimerManager().ClearTimer(invicibilityTimer);

	OnEndInvicibility.Broadcast(this);
}

void APlayerCharacter::HandleMagazineDecal(const int bullets)
{
	if (!MagazineRing)
	{
		return;
	}
	float bulletRate = currentWeapon->GetAmmoRate();
	MagazineRing->SetScalarParameterValue(MagazineRingPercentName, bulletRate);
}

void APlayerCharacter::EnableInfiniteDash(float Duration)
{
	Player_Controller->GetAction<UDashAction>()->SetInfinityDash(true);

	if(InfiniteDashTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InfiniteDashTimer);
	}

	GetWorldTimerManager().SetTimer(InfiniteDashTimer, this, &ThisClass::DisableInfiniteDash, Duration, false);
}

void APlayerCharacter::TogglePersistentInfiniteDash(bool Enable)
{
	Player_Controller->GetAction<UDashAction>()->TogglePersistentInfinityDash(Enable);
}

void APlayerCharacter::DisableInfiniteDash()
{
	Player_Controller->GetAction<UDashAction>()->SetInfinityDash(false);
}
