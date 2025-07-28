#include "Widgets/CustomButtonWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

// ************************** OVERRIDE METHODS ***************************
void UCustomButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text->SetText(Button_Text);
}

void UCustomButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &ThisClass::OnClicked_Notify);
	Button->OnHovered.AddDynamic(this, &ThisClass::OnFocus_Notify);
	Button->OnUnhovered.AddDynamic(this, &ThisClass::OnUnfocus_Notify);
}

void UCustomButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Button->OnClicked.RemoveDynamic(this, &ThisClass::OnClicked_Notify);
	Button->OnHovered.RemoveDynamic(this, &ThisClass::OnFocus_Notify);
	Button->OnUnhovered.RemoveDynamic(this, &ThisClass::OnUnfocus_Notify);
}
// ***********************************************************************



// ************************* BINDING METHODS *****************************
void UCustomButtonWidget::OnFocus_Notify()
{
	PlayFocusSound();

	Toggle_Focus_Color(true);

	PlayFocusAnimation(false);

	OnFocusRecived.Broadcast(this);
}

void UCustomButtonWidget::OnUnfocus_Notify()
{
	Toggle_Focus_Color(false);

	PlayFocusAnimation(true);

	OnFocusMissing.Broadcast(this);
}

void UCustomButtonWidget::OnClicked_Notify()
{
	PlayClickedSound();

	if(Button_Clicked_Animation)
	{
		PlayAnimationForward(Button_Clicked_Animation);
	}

	Button->SetBackgroundColor(Clicked_Color_ButtonBackground);
	Text->SetColorAndOpacity(Clicked_Color_Text);
}
// ***********************************************************************



// ************************** FOCUS METHODS ******************************
void UCustomButtonWidget::Update_Focus_Status(bool bTriggerSound)
{
	if (bTriggerSound)
	{
		PlayFocusSound();
	}

	Toggle_Focus_Color(Button->HasKeyboardFocus());
	
}

void UCustomButtonWidget::Toggle_Focus_Color(bool bIsSelected)
{
	if (bIsSelected)
	{
		Button->SetBackgroundColor(Focus_Color_ButtonBackground);
		Text->SetColorAndOpacity(Focus_Color_Text);
	}
	else
	{
		Button->SetBackgroundColor(Default_Color_ButtonBackground);
		Text->SetColorAndOpacity(Default_Color_Text);
	}
}
// ***********************************************************************



// ************************** PLAY SOUNDS ********************************

void UCustomButtonWidget::PlayFocusSound()
{
	if(Focus_Button_SFX)
	{
		UGameplayStatics::PlaySound2D(this, Focus_Button_SFX);
	}
}

void UCustomButtonWidget::PlayClickedSound()
{
	if (Clicked_Button_SFX)
	{
		UGameplayStatics::PlaySound2D(this, Clicked_Button_SFX);
	}
}

// ************************************************************************



// ************************** PLAY WIDGET ANIMATION ***********************

void UCustomButtonWidget::PlayFocusAnimation(bool bReverse)
{
	if(!Button_Focus_Animation)
	{
		return;
	}

	if(bReverse)
	{
		PlayAnimationReverse(Button_Focus_Animation);
	}
	else
	{
		PlayAnimationForward(Button_Focus_Animation);
	}
}
// ************************************************************************

