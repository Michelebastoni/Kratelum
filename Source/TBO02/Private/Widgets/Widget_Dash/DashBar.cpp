#include "Widgets/Widget_Dash/DashBar.h"
#include "Widgets/GenericProgressBar.h"
#include "Components/ProgressBar.h"

void UDashBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ProgressBar)
	{
		defaulDashColor = ProgressBar->GetFillColorAndOpacity();
		StartToRefill();
	}
}

void UDashBar::CheckProgressBarStatus()
{
	Super::CheckProgressBarStatus();
	
	if (!ProgressBar)
		return;
	
	if (currentProgressBarPercent < lowDashRatio)
	{
		ProgressBar->SetFillColorAndOpacity(dashColorEmpty);
	}
	else
	{
		ProgressBar->SetFillColorAndOpacity(defaulDashColor);
	}
}

void UDashBar::StartToRefill()
{
	SetProgressBarValue(1.f);
}

