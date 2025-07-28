#include "Widgets/Widget_Score/ScoreWidget.h"
#include "RankSystem/ScoreHandler.h"
#include "Widgets/Widget_Reload/QTEReload.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	trapMultiplierText->SetVisibility(ESlateVisibility::Hidden);
}

void UScoreWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(scoreHandler)
	{
		scoreHandler->OnScoreUpdate.RemoveDynamic(this, &ThisClass::UpdateScoreUI);
		scoreHandler->OnScoreComboEnd.RemoveDynamic(this, &ThisClass::UpdateScoreUI);
	}
}

void UScoreWidget::SetUp(AScoreHandler* score)
{
	checkf(score, TEXT("Score not set into %s"), *GetNameSafe(this));

	scoreHandler = score;

	scoreHandler->OnScoreUpdate.AddDynamic(this, &ThisClass::UpdateScoreUI);
	scoreHandler->OnScoreComboEnd.AddDynamic(this, &ThisClass::UpdateScoreUI);

	UpdateScoreUI(scoreHandler, false);
	UpdateCurrentScore();
}

void UScoreWidget::UpdateScoreBar(const float value)
{
	if (scoreBar)
		scoreBar->SetPercent(value);
}

void UScoreWidget::PlayScore_Fade(bool bForwardAnim, float startTime)
{
	if (bForwardAnim)
	{
		PlayAnimation(
			Score_Fade,
			startTime
		);
	}
	else
	{
		PlayAnimation(
			Score_Fade,
			startTime,
			1,
			EUMGSequencePlayMode::Reverse
		);
	}
}

void UScoreWidget::UpdateScoreUI(const AScoreHandler* score, bool bIsTrapKill)
{
	if(!score)
	{
		UE_LOG(LogTemp, Display, TEXT("score handler missing!!"));
		return;
	}

	FFormatNamedArguments args;
	args.Add("KillPoint", FText::AsNumber(score->GetCurrentKillPoint()));
	args.Add("Multiplier", FText::AsNumber(score->GetCurrentScoreMultiplier()));
	args.Add("TrapMultiplier", FText::AsNumber(score->GetCurrentTrapMultiplier()));

	FText kpTxt = FText::Format(NSLOCTEXT("MyNamespace", "MyKey", "+{KillPoint}"), args);
	killPointText->SetText(kpTxt);

	FText multiplierTxt = FText::Format(NSLOCTEXT("MyNamespace", "MyKey", "x{Multiplier}"), args);
	multiplierText->SetText(multiplierTxt);

	FText trapMultiTxt = FText::Format(NSLOCTEXT("MyNamespace", "MyKey", "x{TrapMultiplier}"), args);
	trapMultiplierText->SetText(trapMultiTxt);

	if(scoreTimer.IsValid())
	{
		args.Add("CurrentScore", FText::AsNumber(scoreHandler->GetPreviousScore()));

		FText scoreTxt = FText::Format(NSLOCTEXT("MyNamespace", "MyKey", "{CurrentScore}"), args);
		CurrentScore->SetText(scoreTxt);
	}

	if(bIsTrapKill)
	{
		trapMultiplierText->SetVisibility(ESlateVisibility::Visible);
		OnTrapCombo.Broadcast(score);
	}
	else
	{
		trapMultiplierText->SetVisibility(ESlateVisibility::Hidden);
	}

	GetWorld()->GetTimerManager().SetTimer(
		scoreTimer,
		this,
		&ThisClass::UpdateCurrentScore,
		ScoreTimeApparence,
		false
	);

	OnComboUpdate.Broadcast(score);
}

void UScoreWidget::UpdateCurrentScore()
{
	if (!scoreHandler)
		return;

	FFormatNamedArguments args;
	args.Add("CurrentScore", FText::AsNumber(scoreHandler->GetCurrentScore()));

	FText scoreTxt = FText::Format(NSLOCTEXT("MyNamespace", "MyKey", "{CurrentScore}"), args);
	CurrentScore->SetText(scoreTxt);

	GetWorld()->GetTimerManager().ClearTimer(scoreTimer);
}
