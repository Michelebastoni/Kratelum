#include "Widgets/Widget_Pause/PauseWidget.h"
#include "Widgets/CustomButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Controllers/GenericPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Quit_Widget->OnFocusRecived.AddDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Quit_Widget->OnFocusMissing.AddDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Quit_Button = Quit_Widget->GetButton();

	Resume_Widget->OnFocusRecived.AddDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Resume_Widget->OnFocusMissing.AddDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Resume_Button = Resume_Widget->GetButton();

	Resume_Button->OnClicked.AddDynamic(this, &ThisClass::NotifyResume_Button);
	Quit_Button->OnClicked.AddDynamic(this, &ThisClass::NotifyQuit_Button);
}

void UPauseWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Resume_Widget->OnFocusRecived.RemoveDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Resume_Widget->OnFocusMissing.RemoveDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Resume_Button->OnClicked.RemoveDynamic(this, &ThisClass::NotifyResume_Button);

	Quit_Widget->OnFocusRecived.RemoveDynamic(this, &ThisClass::HandleCustomButtonFocused);
	Quit_Widget->OnFocusMissing.RemoveDynamic(this, &ThisClass::HandleCustomButtonUnfocused);
	Quit_Button->OnClicked.RemoveDynamic(this, &ThisClass::NotifyQuit_Button);

}

void UPauseWidget::Update_Customs_Buttons_Widgets()
{
	Super::Update_Customs_Buttons_Widgets();
	Update_Custom_Button_Focus(Resume_Widget);
	Update_Custom_Button_Focus(Quit_Widget);
}

void UPauseWidget::SetPrimaryWidgetButton()
{
	Current_Custom_Button_Focused = Resume_Widget;
}

void UPauseWidget::SetGameScreenContext()
{
	CurrentPlayerController->SwitchIntoPauseContext();
}

void UPauseWidget::NotifyResume_Button()
{
	ToggleGameScreen(false);
}

void UPauseWidget::NotifyQuit_Button()
{
	GetOwningPlayer()->ConsoleCommand("quit");
}
