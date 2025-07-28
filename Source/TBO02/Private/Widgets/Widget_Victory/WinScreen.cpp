#include "Widgets/Widget_Victory/WinScreen.h"
#include "Widgets/Widget_Loading/LoadingScreen.h"
#include "Widgets/CustomButtonWidget.h"
#include "Widgets/Widget_Score/ScoreDisplay.h"

#include "Controllers/GenericPlayerController.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"


void UWinScreen::NativeConstruct()
{
	Super::NativeConstruct();
	
	Next_Level_Button = NextLevel_Widget->GetButton();

	NextLevel_Widget->OnFocusRecived.AddDynamic(this, &ThisClass::HandleCustomButtonFocused);
	NextLevel_Widget->OnFocusMissing.AddDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Next_Level_Button->OnClicked.AddDynamic(this, &ThisClass::NotifyNext_Level_Button);

	NextLevel_Event.BindDynamic(this, &ThisClass::Go_To_NextLevel);
	Win_Intro_Event.BindDynamic(this, &ThisClass::ShowScore);

}

void UWinScreen::NativeDestruct()
{
	Super::NativeDestruct();

	NextLevel_Widget->OnFocusRecived.RemoveDynamic(this, &ThisClass::HandleCustomButtonFocused);
	NextLevel_Widget->OnFocusMissing.RemoveDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Next_Level_Button->OnClicked.RemoveDynamic(this, &ThisClass::NotifyNext_Level_Button);

	NextLevel_Event.Unbind();
	if(NextLevel_Event.IsBoundToObject(this))
	{
		UnbindFromAnimationFinished(Transition_Animation, NextLevel_Event);
	}

	Win_Intro_Event.Unbind();
	if(Win_Intro_Event.IsBoundToObject(this))
	{
		if(Win_Intro_Animation)
		{
			UnbindFromAnimationFinished(Win_Intro_Animation, Win_Intro_Event);
		}
	}

}

void UWinScreen::Update_Customs_Buttons_Widgets()
{
	Update_Custom_Button_Focus(NextLevel_Widget);
	Super::Update_Customs_Buttons_Widgets();
}

void UWinScreen::SetPrimaryWidgetButton()
{
	Current_Custom_Button_Focused = NextLevel_Widget;
}

void UWinScreen::NotifyNext_Level_Button()
{
	checkf(!CurrentNextLevel.IsNull(), TEXT("CurrentNextLevel is NULL. \nSouce: %s"), *GetName())

	ToggleControllerSettings(false);

	if(Transition_Animation)
	{
		BindToAnimationFinished(Transition_Animation, NextLevel_Event);
		PlayAnimationForward(Transition_Animation);
	}
	else
	{
		Go_To_NextLevel();
	}
}

void UWinScreen::Go_To_NextLevel()
{
	Loading_Screen = CurrentPlayerController->GetLoadingScreen();

	OnGameScreenDisable.Broadcast(this);

	checkf(Loading_Screen, TEXT("Loading Screen referenced into [%s] is NUll, Souce: [%s]"), *GetNameSafe(CurrentPlayerController), *GetName());

	Loading_Screen->SetAffectedByTimeDilation(true);
	Loading_Screen->OpenLevelByLevel(CurrentNextLevel);

	CurrentPlayerController->DisableInput(CurrentPlayerController);
}

void UWinScreen::StartShowScore()
{
	if(!Score_Display_Widget)
	{
		return;
	}

	if(!Win_Intro_Animation)
	{
		ShowScore();
	}
	else
	{
		
		BindToAnimationFinished(Win_Intro_Animation, Win_Intro_Event);
		PlayAnimationForward(Win_Intro_Animation);
	}
}

void UWinScreen::ShowScore()
{
	if(Score_Display_Widget)
	{
		Score_Display_Widget->StartScoreInterpolation();
	}
}
