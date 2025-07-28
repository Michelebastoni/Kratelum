#include "Controllers/GenericPlayerController.h"
#include "CharacterLogic/GenericCharacter.h"
#include "CharacterLogic/PlayerCharacter.h"

#include "Widgets/Widget_Reload/QTEReload.h"
#include "Widgets/GameScreen.h"
#include "Widgets/Widget_Loading/LoadingScreen.h"
#include "Widgets/Widget_Cursor/CursorWidget.h"
#include "Widgets/Widget_HUD/HUDWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "IntegritySystem/Integrity.h"
#include "IntegritySystem/UI/IntegirtyWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Weapons/Weapon.h"
#include "Weapons/MeleeComponent.h"

#include "InteractionSystem/Components/InteractionComponent.h"
#include "InteractionSystem/Interfaces/Interactor.h"
#include "InteractionSystem/Interfaces/Interactable.h"

#include "Controllers/Actions/ActionBase.h"
#include "Controllers/Actions/DashAction.h"

#include "Kismet/GameplayStatics.h"

AGenericPlayerController::AGenericPlayerController() : bIsLastInputAPad(false)
{
	PrimaryActorTick.bCanEverTick = true;
	SetGenericTeamId(FGenericTeamId(0));
}

void AGenericPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
		// @todo notify perception system that a controller changed team ID
	}
}

FGenericTeamId AGenericPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void AGenericPlayerController::Tick(float deltaSeconds)
{
	// if there's no character stop here
	if (!Current_Character)
	{
#if !UE_BUILD_SHIPPING
		if (showMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Character missing!"));
		}
#endif
		return;
	}

	if (Reload_Screen)
	{
		Reload_Screen->UpdateSliderLocationAtLocation(Current_Character->GetActorLocation());
	}

	if (Cursor_Widget)
	{
		float x;
		float y;
		GetMousePosition(x, y);

		FVector2D NewPosition = FVector2D{x, y};
		NewPosition /= UWidgetLayoutLibrary::GetViewportScale(GetWorld());

		Cursor_Widget->UpdateCursorLocation(NewPosition);
	}
}

bool AGenericPlayerController::InputKey(const FInputKeyParams& Params)
{
	OnInputReceived.Broadcast(Params.Key, Params.IsGamepad());

	bIsLastInputAPad = Params.IsGamepad();

	if (!Cursor_Widget)
	{
		return Super::InputKey(Params);
	}

	if (Params.IsGamepad())
	{
		Cursor_Widget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Cursor_Widget->SetVisibility(ESlateVisibility::Visible);
	}

	return Super::InputKey(Params);
}

void AGenericPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get subsystem from local player subsystem
	subSystem =
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	// Add mapping context
	subSystem->AddMappingContext(GameContext, 0);

	// Subscribe into character dead event
	if (Current_Character)
	{
		Current_Character->OnCharacterDeath.AddDynamic(this, &ThisClass::HandleCharacterDeath);
	}

	// Create Loading widget
	if (IsValid(Loading_Screen_Class))
	{
		Loading_Screen = Cast<ULoadingScreen>(CreateWidget(this, Loading_Screen_Class));
		if (Loading_Screen)
		{
			Loading_Screen->AddToViewport(2);
		}
	}

	// Create Pause widget
	if (IsValid(PauseClass))
	{
		Pause_Screen = Cast<UGameScreen>(CreateWidget(this, PauseClass));
		if (Pause_Screen)
		{
			Pause_Screen->AddToViewport(1);
		}
	}

	// Create Cursor widget
	if (IsValid(Cursor_Widget_Class))
	{
		Cursor_Widget = Cast<UCursorWidget>(CreateWidget(this, Cursor_Widget_Class));
		if (Cursor_Widget)
		{
			Cursor_Widget->AddToViewport();
		}
	}

	// Create and add to viewport the integrity widget
	if (IsValid(HUDClass))
	{
		HUDWidget = Cast<UHUDWidget>(CreateWidget(this, HUDClass));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	// Create Reload widget
	if (IsValid(reloadWidgetClass))
	{
		Reload_Screen = Cast<UQTEReload>(CreateWidget(this, reloadWidgetClass));
		if (Reload_Screen)
		{
			Reload_Screen->AddToViewport();
		}
	}

	// Setup all Actions
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		Actions[i]->SetUp(this);
	}

	// Call event
	OnFinishSetUp.Broadcast(this);

	UDashAction* Action = GetAction<UDashAction>();
}

void AGenericPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Current_Character)
	{
		Current_Character->OnCharacterDeath.RemoveDynamic(this, &ThisClass::HandleCharacterDeath);
	}

	// ShutDown all actions
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		Actions[i]->ShutDown();
	}

	// Remove input mapping context
	subSystem->RemoveMappingContext(GameContext);
}

void AGenericPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Get Enhanced input from input component
	enhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(InputComponent);


	enhancedInputComponent->BindAction(
		SkipAction,
		ETriggerEvent::Started,
		this,
		&ThisClass::HandleSkipCinematic
	);
}

void AGenericPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	Current_Character = Cast<APlayerCharacter>(aPawn);
}

void AGenericPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	Current_Character = nullptr;
}

void AGenericPlayerController::HandleCharacterDeath(AGenericCharacter* controlledCharacter, AWeapon* currentWeapon,
                                                    const AActor* harmer, UIntegrity* integrity)
{
	controlledCharacter->ToggleCharacterRotation(false);
	bShowMouseCursor = true;
	subSystem->RemoveMappingContext(GameContext);
}

UActionBase* AGenericPlayerController::GetActionByClass(TSubclassOf<UActionBase> ActionClass) const
{
	for (UActionBase* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

template <typename T>
T* AGenericPlayerController::GetAction() const
{
	for (int i = 0; i < Actions.Num(); i++)
	{
		if (!Actions[i])
		{
			continue;
		}

		if (T* ActionCasted = Cast<T>(Actions[i]))
		{
			return ActionCasted;
		}
	}

	return nullptr;
}

void AGenericPlayerController::SwitchContext(UInputMappingContext* newInputMap)
{
	checkf(subSystem, TEXT("SubSystem missing! \nSource: [%s]"), *GetNameSafe(this))

	checkf(GameContext, TEXT("Game Context missing! \nSource: [%s]"), *GetNameSafe(this));

	checkf(UIContext, TEXT("UI Context missing \nSource: [%s]"), *GetNameSafe(this));

	checkf(HUBContext, TEXT("HUB Context missing \nSource: [%s]"), *GetNameSafe(this));

	if (subSystem->HasMappingContext(GameContext))
	{
		subSystem->RemoveMappingContext(GameContext);
	}

	if (subSystem->HasMappingContext(UIContext))
	{
		subSystem->RemoveMappingContext(UIContext);
	}

	if (subSystem->HasMappingContext(HUBContext))
	{
		subSystem->RemoveMappingContext(HUBContext);
	}

	if (subSystem->HasMappingContext(CinematicContext))
	{
		subSystem->RemoveMappingContext(CinematicContext);
	}

	if (subSystem->HasMappingContext(PauseContext))
	{
		subSystem->RemoveMappingContext(PauseContext);
	}

	if (newInputMap)
	{
		subSystem->AddMappingContext(newInputMap, 0);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				45,
				100.0f,
				FColor::Orange,
				FString::Printf(TEXT("New input is missing. \nSource: [%s]"), *GetNameSafe(this))
			);
		}
	}
}

void AGenericPlayerController::HandleSkipCinematic()
{
	OnSkipCinematic.Broadcast();
}
