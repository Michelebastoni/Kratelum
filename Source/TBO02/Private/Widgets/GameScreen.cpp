#include "Widgets/GameScreen.h"
#include "Widgets/Widget_Loading/LoadingScreen.h"
#include "Widgets/CustomButtonWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Controllers/GenericPlayerController.h"
#include "Animation/WidgetAnimation.h"

void UGameScreen::NativeConstruct()
{
	Super::NativeConstruct();

	bIsGameScreenActive = false;

	CurrentPlayerController = Cast<AGenericPlayerController>(GetOwningPlayer());

	CurrentPlayerController->OnInputReceived.AddDynamic(this, &ThisClass::HandleInputs);

	// Get buttons from widget
	HUB_Button = HUB_Widget->GetButton();
	Retry_Button = Retry_Widget->GetButton();

	// Bind buttons events
	HUB_Widget->OnFocusRecived.AddDynamic(this, &ThisClass::HandleCustomButtonFocused);
	HUB_Widget->OnFocusMissing.AddDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	HUB_Button->OnClicked.AddDynamic(this, &ThisClass::NotifyHUB_Button);

	Retry_Widget->OnFocusRecived.AddDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Retry_Widget->OnFocusMissing.AddDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Retry_Button->OnClicked.AddDynamic(this, &ThisClass::NotifyRetry_Button);

	// Bind transtion finished event
	Retry_Event.BindDynamic(this, &ThisClass::ReloadLevel);
	HUB_Event.BindDynamic(this, &ThisClass::Return_To_HUB);

	SetVisibility(ESlateVisibility::Collapsed);

	SetPrimaryWidgetButton();

	SetIsFocusable(true);

}

void UGameScreen::NativeDestruct()
{
	Super::NativeDestruct();

	CurrentPlayerController->OnInputReceived.RemoveDynamic(this, &ThisClass::HandleInputs);

	// Unbind buttons events
	HUB_Widget->OnFocusRecived.RemoveDynamic(this, &ThisClass::HandleCustomButtonFocused);
	HUB_Widget->OnFocusMissing.RemoveDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	HUB_Button->OnClicked.RemoveDynamic(this, &ThisClass::NotifyHUB_Button);

	Retry_Widget->OnFocusRecived.RemoveDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Retry_Widget->OnFocusMissing.RemoveDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Retry_Button->OnClicked.RemoveDynamic(this, &ThisClass::NotifyRetry_Button);

	// Unbind transtion finished event
	Retry_Event.Unbind();
	HUB_Event.Unbind();

	if(Retry_Event.IsBoundToObject(this))
	{
		UnbindFromAnimationFinished(Transition_Animation, Retry_Event);
	}

	if(HUB_Event.IsBoundToObject(this))
	{
		UnbindFromAnimationFinished(Transition_Animation, Retry_Event);
	}

}

FReply UGameScreen::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// If mouse is moving show the cursor
	if(!InMouseEvent.GetCursorDelta().IsNearlyZero())
	{
		SetCursor(EMouseCursor::Default);
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UGameScreen::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SetPrimaryButtonFocus();

	Update_Customs_Buttons_Widgets();

	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

FReply UGameScreen::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SetPrimaryButtonFocus();

	Update_Customs_Buttons_Widgets();

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UGameScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Update_Customs_Buttons_Widgets();

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UGameScreen::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Update_Customs_Buttons_Widgets();

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}


void UGameScreen::Update_Customs_Buttons_Widgets()
{
	Update_Custom_Button_Focus(HUB_Widget);
	Update_Custom_Button_Focus(Retry_Widget);
}

void UGameScreen::HandleCustomButtonFocused(UCustomButtonWidget* widgetToCheck)
{
	if(!widgetToCheck)
	{
		return;
	}

	if(!Current_Custom_Button_Focused)
	{
		Current_Custom_Button_Focused = widgetToCheck;
		widgetToCheck->GetButton()->SetFocus();
		return;
	}

	if(Current_Custom_Button_Focused == widgetToCheck)
	{
		return;
	}

	Current_Custom_Button_Focused->Toggle_Focus_Color(false);
	Current_Custom_Button_Focused->PlayFocusAnimation(true);
	Current_Custom_Button_Focused = widgetToCheck;
	widgetToCheck->GetButton()->SetFocus();
}

void UGameScreen::HandleCustomButtonUnfocused(UCustomButtonWidget* widgetToCheck)
{
	if(Current_Custom_Button_Focused == widgetToCheck)
	{
		Current_Custom_Button_Focused = nullptr;
	}
}

void UGameScreen::Update_Custom_Button_Focus(UCustomButtonWidget* widgetToCheck)
{
	if(!widgetToCheck)
	{
		return;
	}

	const UButton* widgetButton = widgetToCheck->GetButton();

	if(widgetButton->HasAnyUserFocus())
	{

		if(!Current_Custom_Button_Focused)
		{
			widgetToCheck->PlayAnimationForward(widgetToCheck->GetFocusAnimation());
			
			widgetToCheck->Update_Focus_Status();

			Current_Custom_Button_Focused = widgetToCheck;

			OnButtonFocusRecived.Broadcast(widgetToCheck);

			return;
		}

		if(Current_Custom_Button_Focused == widgetToCheck)
		{
			return;
		}
		else
		{
			Current_Custom_Button_Focused->PlayAnimationReverse(Current_Custom_Button_Focused->GetFocusAnimation());
			
			widgetToCheck->Update_Focus_Status();

			widgetToCheck->PlayAnimationForward(widgetToCheck->GetFocusAnimation());

			Current_Custom_Button_Focused = widgetToCheck;

			OnButtonFocusRecived.Broadcast(widgetToCheck);
		}
	}
	else
	{
		widgetToCheck->Update_Focus_Status(false);
		OnButtonFocusLost.Broadcast(widgetToCheck);
	}
	
}

void UGameScreen::SetPrimaryButtonFocus()
{
	if(Current_Custom_Button_Focused)
	{
		Current_Custom_Button_Focused->GetButton()->SetFocus();
		Current_Custom_Button_Focused->Toggle_Focus_Color(true);
		Current_Custom_Button_Focused->PlayFocusAnimation(false);
	}
	else
	{
		SetPrimaryWidgetButton();
		SetPrimaryButtonFocus();
	}
}

void UGameScreen::SetPrimaryWidgetButton()
{
	Current_Custom_Button_Focused = Retry_Widget;
}

void UGameScreen::SetGameScreenContext()
{
	CurrentPlayerController->SwitchIntoUIContext();
}


void UGameScreen::NotifyHUB_Button()
{	
	CurrentPlayerController->DisableInput(CurrentPlayerController);

	ToggleControllerSettings(false);
	
	if(Transition_Animation)
	{
		// --> Bind and Play HUB transtion
		BindToAnimationFinished(Transition_Animation, HUB_Event);
		PlayAnimationForward(Transition_Animation);
	}
	else
	{
		Return_To_HUB();
	}
	
}

void UGameScreen::NotifyRetry_Button()
{
	CurrentPlayerController->DisableInput(CurrentPlayerController);

	ToggleControllerSettings(false);

	if(Transition_Animation)
	{
		// --> Bind and Play retry transtion
		BindToAnimationFinished(Transition_Animation, Retry_Event);
		PlayAnimationForward(Transition_Animation);
	}
	else
	{
		ReloadLevel();
	}
}

void UGameScreen::ReloadLevel()
{
	OnGameScreenDisable.Broadcast(this);

	FString LevelName = UGameplayStatics::GetCurrentLevelName(this);

	Loading_Screen = CurrentPlayerController->GetLoadingScreen();

	checkf(Loading_Screen, TEXT("Loading Screen refered into [&s] is NUll, Souce: [%s]"), *GetNameSafe(CurrentPlayerController), *GetName());

	Loading_Screen->SetAffectedByTimeDilation(true);
	Loading_Screen->OpenLevelByName(FName(LevelName));
}

void UGameScreen::Return_To_HUB()
{
	OnGameScreenDisable.Broadcast(this);

	Loading_Screen = CurrentPlayerController->GetLoadingScreen();

	checkf(Loading_Screen, TEXT("Loading Screen refered into [&s] is NUll, Souce: [%s]"), *GetNameSafe(CurrentPlayerController), *GetName());
	checkf(!HUB_Level.IsNull(), TEXT("Cant find HUB_Level into [%s], Source [%s]"), *HUB_Level.GetLongPackageName(), *GetName());

	Loading_Screen->SetAffectedByTimeDilation(true);
	Loading_Screen->OpenLevelByName(FName(*HUB_Level.GetLongPackageName()));
}

void UGameScreen::HandleInputs(const FKey& Key, bool isGamepadInput)
{
	if(!bIsGameScreenActive)
	{
		return;
	}

	if(isGamepadInput)
	{
		SetCursor(EMouseCursor::None);
		FSlateApplication::Get().QueryCursor();
		CurrentPlayerController->SetShowMouseCursor(false);
	}
	else
	{
		SetCursor(EMouseCursor::Default);
		CurrentPlayerController->SetShowMouseCursor(true);
	}
}


void UGameScreen::ToggleGameScreen(bool bEnable)
{
	if (bEnable)
	{
		UGameplayStatics::SetGlobalTimeDilation(this, KINDA_SMALL_NUMBER);

		bIsGameScreenActive = true;

		ToggleControllerSettings(bIsGameScreenActive);

		SetVisibility(ESlateVisibility::Visible);

		SetPrimaryButtonFocus();

		OnGameScreenEnable.Broadcast(this);
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

		SetVisibility(ESlateVisibility::Collapsed);

		bIsGameScreenActive = false;

		ToggleControllerSettings(bIsGameScreenActive);

		OnGameScreenDisable.Broadcast(this);
	}
}

void UGameScreen::ToggleControllerSettings(bool bEnable)
{
	if(!CurrentPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller missing on %s"), *GetNameSafe(this));
		return;
	}

	if(bEnable)
	{		
		CurrentPlayerController->SetInputMode(FInputModeGameAndUI());

		SetGameScreenContext();

		bool bIsPadInput = CurrentPlayerController->GetIsLastInputAPad();

		CurrentPlayerController->bShowMouseCursor = false;
		
	}
	else
	{		
		CurrentPlayerController->SwitchIntoGameContext();

		CurrentPlayerController->bShowMouseCursor = false;

		CurrentPlayerController->SetInputMode(FInputModeGameOnly());
		
	}
}
