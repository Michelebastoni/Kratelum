#include "Widgets/Widget_Score/ScoreDisplay.h"
#include "Components/TextBlock.h"

#include "Kismet/KismetMathLibrary.h"
#include "RankSystem/ScoreHandler.h"

void UScoreDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	bIsScoreUpdating = false;

	if(!ScoreHandler)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				69,
				100.0f,
				FColor::Orange,
				FString::Printf(TEXT("Score Handler is null \n Source[%s]"), *GetName())
			);
		}

		SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UScoreDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(End_Interpolation_Time > GetWorld()->GetRealTimeSeconds())
	{

		Current_Score_Value = (int)FMath::GetMappedRangeValueClamped(
			FVector2D{ Start_Interpolation_Time, End_Interpolation_Time },
			FVector2D{ 0.0f, (float)Score_To_Reach },
			GetWorld()->GetRealTimeSeconds()
		);

	}
	else
	{
		// Be sure to update the current value to score to reach
		Current_Score_Value = Score_To_Reach;

		// Call event
		if(bIsScoreUpdating)
		{
			OnScoreReached.Broadcast(Current_Score_Value);
			bIsScoreUpdating = false;
		}
	}

	Score_Text->SetText(FText::AsNumber(Current_Score_Value));
}

void UScoreDisplay::StartScoreInterpolation()
{
	OnScoreStarted.Broadcast(Current_Score_Value);

	Start_Interpolation_Time = GetWorld()->GetTimeSeconds();
	End_Interpolation_Time = Start_Interpolation_Time + GetInterpolationTime();
	Score_To_Reach = ScoreHandler->GetCurrentScore();

	UE_LOG(LogTemp, Log, TEXT("Time To Reach score: %.2f"), GetInterpolationTime());

	// Set bool status 
	bIsScoreUpdating = true;
}

float UScoreDisplay::GetInterpolationTime_Implementation()
{
	return 0.0f;
}
