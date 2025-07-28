#include "Controllers/Actions/DashAction.h"
#include "Controllers/GenericPlayerController.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "IntegritySystem/Integrity.h"

void UDashAction::SetUp(AGenericPlayerController* controller)
{
	Super::SetUp(controller);

	DashData.bPersistentInfinityDash = false;

	SetUpData();

	SetDashCount(DashData.MaxDashCount);

	if (DashData.DashVFX)
	{
		dashVFXComponent =
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				DashData.DashVFX,
				Player->GetRootComponent(),
				"None",
				FVector{},
				FRotator{},
				EAttachLocation::KeepRelativeOffset,
				false,
				false
			);
	}

	// Save dash collision settings
	for (const TPair<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& pair : DashData.
	     DashCollisionSettings)
	{
		TEnumAsByte<ECollisionResponse> response = Player->GetCapsuleComponent()->
		                                                   GetCollisionResponseToChannel(pair.Key);
		savedDashCollisionSettings.Add(pair.Key, response);
	}

	Player->GetMesh()->bWaitForParallelClothTask = true;

	IntegrityComponent = Player->GetIntegrityComponent();
}

void UDashAction::ShutDown()
{
	Super::ShutDown();

	if (world)
	{
		world->GetTimerManager().ClearTimer(dashCooldownTimer);
	}
}

void UDashAction::Perform(const FInputActionValue& val)
{
	Super::Perform(val);

	if (!GetStrongerCondition())
	{
		if (DashData.DashCount <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot perform dash, dash count: %d"), dashCount);
			return;
		}
	}

	if (!GetStrongerCondition())
	{
		RemoveDashCount();
	}

	ToggleDashSettings(true);

	MakeDash(DashData.DashDistance);

	IntegrityComponent->AddImmortalitySource();

	// Play dash vfx
	if (dashVFXComponent)
	{
		dashVFXComponent->SetActive(true, false);
	}

	// Play dash audio
	if (DashData.DashSFX)
	{
		UGameplayStatics::PlaySound2D(world, DashData.DashSFX);
	}

	// Update dashing status
	bIsDashing = true;

	// Start dash timer
	FTimerHandle dashTimer;
	world->GetTimerManager().SetTimer(
		dashTimer,
		this,
		&ThisClass::OnDashFinished,
		0.2f,
		false
	);
	

	OnDashPerformed.Broadcast(DashData.DashCount);
}

void UDashAction::SetUpData()
{
	DashData.bInfinityDash = bInfinityDash;

	DashData.MaxDashCount = maxDashCount;

	DashData.DashCount = dashCount;

	DashData.DashCoolDown = dashCoolDown;

	DashData.DashDistance = dashDistance;

	DashData.DashMovethreshold = dashMovethreshold;

	DashData.DashCoolDownMultiplier = dashCoolDownMultiplier;

	if (dashReadySFX)
	{
		DashData.DashReadySFX = dashReadySFX;
	}

	if (dashSFX)
	{
		DashData.DashSFX = dashSFX;
	}

	if (dashVFX)
	{
		DashData.DashVFX = dashVFX;
	}

	DashData.DashCollisionSettings = dashCollisionSettings;

	DashData.DashAOEDamage = dashAOEDamage;

	DashData.DashAOERadius = dashAOERadius;

	DashData.DashObjectDetection = dashObjectDetection;

	DashData.DashExplosionSFX = dashExplosionSFX;

	DashData.DashExplosionVFX = dashExplosionVFX;
}

bool UDashAction::GetStrongerCondition() const
{
	if(DashData.bPersistentInfinityDash)
	{
		return DashData.bPersistentInfinityDash;
	}

	if(DashData.bInfinityDash)
	{
		return DashData.bInfinityDash;
	}

	return false;
}

void UDashAction::MakeDash(const float distance)
{
	// Prepare data
	FVector2D Input2D = FVector2D(
		Player->GetLastMovementInputVector().X,
		Player->GetLastMovementInputVector().Y
	);

	FVector DashDirection = (FVector::ForwardVector * Input2D.X + FVector::RightVector * Input2D.Y).GetSafeNormal();

	bool isMoving = !DashDirection.IsNearlyZero();

	// Make Dash
	Player->LaunchCharacter(
		(
			isMoving ? DashDirection : Player->GetActorForwardVector()
		)
		* distance,
		false,
		false
	);
}

void UDashAction::ToggleDashSettings(bool enable)
{
	if (!Player)
	{
		return;
	}

	if (enable)
	{
		for (const TPair<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& pair : DashData.
		     DashCollisionSettings)
		{
			Player->GetCapsuleComponent()->
			        SetCollisionResponseToChannel(
				        pair.Key,
				        pair.Value
			        );
		}
	}
	else
	{
		for (const TPair<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& pair :
		     savedDashCollisionSettings)
		{
			Player->GetCapsuleComponent()->
			        SetCollisionResponseToChannel(
				        pair.Key,
				        pair.Value
			        );
		}
	}
}

float UDashAction::GetDashCoolDownTimer() const
{
	float NormalTime = DashData.DashCoolDown * DashData.DashCoolDownMultiplier;

	if (!dashCooldownTimer.IsValid())
	{
		return NormalTime;
	}

	float CurrentTimeElapsed = world->GetTimerManager().GetTimerElapsed(dashCooldownTimer);

	float NewTime = NormalTime - CurrentTimeElapsed;

	if (NewTime <= 0)
	{
		return 0.0f;
	}

	return NewTime;
}

void UDashAction::SetDashCoolDownMultiplier(const float newMultplier)
{
	DashData.DashCoolDownMultiplier = newMultplier;

	if (!world)
	{
		return;
	}

	if (dashCooldownTimer.IsValid())
	{
		world->GetTimerManager().ClearTimer(dashCooldownTimer);

		world->GetTimerManager().SetTimer(
			dashCooldownTimer,
			this,
			&ThisClass::OnDashCoolDownFinished,
			GetDashCoolDownTimer(),
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("New Time: %.2f"), GetDashCoolDownTimer());

	OnDashCooldownChanged.Broadcast(this);
}

void UDashAction::SetDashVFX(UNiagaraSystem* NewVFX)
{
	if (!NewVFX)
	{
		return;
	}

	DashData.DashVFX = NewVFX;
	dashVFXComponent->SetAsset(DashData.DashVFX);
}

void UDashAction::SetDashCount(const int newDashCount)
{
	int value = newDashCount;

	DashData.DashCount = FMath::Clamp(value, 0, DashData.MaxDashCount);

	if (bEnableDebugMessage)
	{
		UE_LOG(LogTemp, Display, TEXT("Dash count: %d"), DashData.DashCount);
	}
}

void UDashAction::OnDashCoolDownFinished()
{
	if (!world)
	{
		return;
	}

	if (bEnableDebugMessage)
	{
		UE_LOG(LogTemp, Display, TEXT("Dash reloaded"));
	}

	world->GetTimerManager().ClearTimer(dashCooldownTimer);

	AddDashCount();

	// Start timer if dash count is not max count
	if (DashData.DashCount < DashData.MaxDashCount)
	{
		world->GetTimerManager().SetTimer(
			dashCooldownTimer,
			this,
			&ThisClass::OnDashCoolDownFinished,
			DashData.DashCoolDown * DashData.DashCoolDownMultiplier,
			false
		);
	}

	// Play Dash_Ready_SFX
	if (DashData.DashReadySFX && world)
	{
		UGameplayStatics::PlaySound2D(world, DashData.DashReadySFX);
	}

	// Call event
	OnDashCooldownEnd.Broadcast(DashData.DashCount);
}

void UDashAction::OnDashFinished()
{
	if (bEnableDebugMessage)
	{
		UE_LOG(LogTemp, Display, TEXT("Dash End"));
	}

	ToggleDashSettings(false);

	IntegrityComponent->RemoveImmortalitySource();

	if (dashVFXComponent)
	{
		dashVFXComponent->SetActive(false, false);
	}

	// Start dash cooldown timer
	if (!dashCooldownTimer.IsValid())
	{
		if (!world)
		{
			return;
		}

		world->GetTimerManager().SetTimer(
			dashCooldownTimer,
			this,
			&ThisClass::OnDashCoolDownFinished,
			DashData.DashCoolDown * DashData.DashCoolDownMultiplier,
			false
		);
	}

	if (Player)
	{
		// Make damage
		Player->MakeAOEDamage(
			DashData.DashAOERadius,
			DashData.DashAOEDamage,
			DashData.DashObjectDetection,
			bDrawSphereDamageDebug,
			DashData.DashExplosionSFX,
			DashData.DashExplosionVFX
		);
	}

	bIsDashing = false;

	OnDashEnd.Broadcast(this);
}
