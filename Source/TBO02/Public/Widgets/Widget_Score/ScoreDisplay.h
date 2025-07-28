#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreDisplay.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreDisplaySignature, const int, currentScore);

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UScoreDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FScoreDisplaySignature OnScoreReached;

	UPROPERTY(BlueprintAssignable)
	FScoreDisplaySignature OnScoreStarted;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Score_Text;

	int Current_Score_Value = 0;
	int Score_To_Reach = 0;
	float Start_Interpolation_Time = 0.0f;
	float End_Interpolation_Time = 0.0f;
	float Time_ToInterpolate = 0.0f;

	uint8 bIsScoreUpdating : 1;
	TObjectPtr<class AScoreHandler> ScoreHandler;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UFUNCTION(BlueprintCallable)
	void StartScoreInterpolation();
	
	// ----------------------------- Setter -------------------------------

	UFUNCTION(BlueprintCallable)
	void SetScoreHandler(class AScoreHandler* NewScoreHandler) { ScoreHandler = NewScoreHandler; }

	// ---------------------------------------------------------------------

	// ----------------------------- Getter -------------------------------
	
	UFUNCTION(BlueprintPure)
	class AScoreHandler* GetScoreHandler() const { return ScoreHandler; }

	UFUNCTION(BlueprintPure)
	class UTextBlock* GetScore_Text() const { return Score_Text; }

	UFUNCTION(BlueprintPure)
	int GetCurrent_Score_Value() const { return Current_Score_Value; }

	UFUNCTION(BlueprintNativeEvent)
	float GetInterpolationTime();
	// --------------------------------------------------------------------
};
