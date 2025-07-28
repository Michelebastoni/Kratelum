#include "Widgets/GenericProgressBar.h"
#include "Components/ProgressBar.h"

void UGenericProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (progressBarIsChanging && ProgressBar->GetPercent() > progressBarAfterChange)
	{
		progressBarTimeElapsed += InDeltaTime;
		currentProgressBarPercent = 0;
		
		if (isIncreasing)
		{
			currentProgressBarPercent = FMath::Lerp(progressBarAfterChange, progressBarBeforeChange, progressBarTimeElapsed / changeProgressBarDuration);
			
			if (currentProgressBarPercent >= progressBarBeforeChange)
			{
				ProgressBar->SetPercent(progressBarBeforeChange);
				progressBarIsChanging = false;
				isIncreasing = false; // don't need to but more correct
			}
			else
			{
				if (ProgressBar)
				{
					ProgressBar->SetPercent(currentProgressBarPercent);
				}
			}
		}
		else
		{
			currentProgressBarPercent = FMath::Lerp(progressBarBeforeChange, progressBarAfterChange, progressBarTimeElapsed / changeProgressBarDuration);

			if (currentProgressBarPercent <= progressBarAfterChange)
			{
				ProgressBar->SetPercent(progressBarAfterChange);
				progressBarIsChanging = false;
			}
			else
			{
				if (ProgressBar)
				{
					ProgressBar->SetPercent(currentProgressBarPercent);
				}
			}
		}
		CheckProgressBarStatus();
	}
}

void UGenericProgressBar::SetProgressBarValue(float newRatio)
{
	progressBarIsChanging = true;
	progressBarTimeElapsed = 0;
	
	if (currentRatio > newRatio)
	{
		progressBarBeforeChange = currentRatio;
		progressBarAfterChange = newRatio;
		isIncreasing = false;
	}
	else
	{
		progressBarBeforeChange = newRatio;
		progressBarAfterChange = currentRatio;
		isIncreasing = true;
	}
	
	ProgressBar->SetPercent(progressBarBeforeChange); // uso solo per controllare Bug nella UI (se si può lo lascerei)

	if (changeProgressBarDuration < checkMin)
	{
		changeProgressBarDuration = checkMin;
	}
	
	if (progressBarAfterChange < checkMin)
	{
		progressBarAfterChange = checkMin;
	}
	
	if (progressBarChangeRate > 0)
	{
		progressBarChangeRate = (progressBarBeforeChange - progressBarAfterChange) / changeProgressBarDuration;
	}
	else
	{
		progressBarChangeRate = 0;
	}

	currentRatio = newRatio;
}

void UGenericProgressBar::CheckProgressBarStatus()
{
	// UE_LOG(LogTemp, Warning, TEXT("Active function: <CheckProgressBarStatus()>"));
}

void UGenericProgressBar::ForceSetProgressBar(float newValue)
{ // to start the game even for integrity
	currentRatio = newValue;
	currentProgressBarPercent = newValue;
	if (ProgressBar)
		ProgressBar->SetPercent(newValue);
}

