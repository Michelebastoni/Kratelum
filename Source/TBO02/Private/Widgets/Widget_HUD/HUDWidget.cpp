#include "Widgets/Widget_HUD/HUDWidget.h"
#include "Animation/WidgetAnimation.h"

void UHUDWidget::PlayFadeAnimation(bool bForwardAnim, float startTime)
{
	if(bForwardAnim)
	{
		PlayAnimation(
			HUD_Fade,
			startTime
		);
	}
	else
	{
		PlayAnimation(
			HUD_Fade,
			startTime,
			1,
			EUMGSequencePlayMode::Reverse
		);
	}
}
