#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreWidgetSignature, const class AScoreHandler*, scoreHandler);

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FScoreWidgetSignature OnTrapCombo;

	UPROPERTY(BlueprintAssignable)
	FScoreWidgetSignature OnComboUpdate;

private:
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UProgressBar> scoreBar;
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> killPointText;
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> multiplierText;
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> trapMultiplierText;
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> CurrentScore;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> Score_Fade;

	UPROPERTY(EditDefaultsOnly, Category = "Score")
	float ScoreTimeApparence = 1.0f;
	
	
	UPROPERTY(Transient)
	TObjectPtr<class AScoreHandler> scoreHandler;
	FTimerHandle scoreTimer;

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;

public:
	UFUNCTION(BlueprintPure)
	class UProgressBar* GetScoreBar() const { return scoreBar; }

	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetScore_Fade_Animation() const { return Score_Fade; }

	UFUNCTION(BlueprintPure)
	class UTextBlock* GetTrapMultiplierText() const { return trapMultiplierText; }

	UFUNCTION()
	void SetUp(class AScoreHandler* score);

	UFUNCTION()
	void UpdateScoreBar(const float value);

	UFUNCTION(BlueprintCallable)
	void PlayScore_Fade(bool bForwardAnim, float startTime);
private:
	UFUNCTION()
	void UpdateScoreUI(const class AScoreHandler* score, bool bIsTrapKill);
	
	UFUNCTION()
	void UpdateCurrentScore();
};
