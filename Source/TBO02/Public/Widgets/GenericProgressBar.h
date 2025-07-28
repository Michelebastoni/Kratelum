#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericProgressBar.generated.h"

UCLASS(Abstract)
class TBO02_API UGenericProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* ProgressBar;
	float currentProgressBarPercent = 0;
	
	UPROPERTY(EditAnywhere, Category = "ProgressBar",  meta = (UIMin = 0.00001f))
	float changeProgressBarDuration = 1.0f;
private:
	
	float checkMin = 0.00001f;
	bool progressBarIsChanging = false;
	float progressBarChangeRate = 0;
	float progressBarTimeElapsed = 0;
	float progressBarBeforeChange = 0;
	float progressBarAfterChange = 0;
	bool isIncreasing = false;
	float currentRatio;


public:
	UFUNCTION(BlueprintCallable)
	float GetProgressBarDuration() const { return changeProgressBarDuration; }

	UFUNCTION(BlueprintCallable)
	class UProgressBar* GetProgressBar() const { return ProgressBar; }
	
	UFUNCTION(Category = "ProgressBar")
	void SetProgressBarDuration(float newDuration) { changeProgressBarDuration = newDuration; }
	
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetProgressBarValue(float newRatio);
	
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void ForceSetProgressBar(float newValue);
	
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	virtual void CheckProgressBarStatus();
	
	UFUNCTION(Category = "ProgressBar")
	void ForceSetCurrentRatio(float newCurrentRatio) { currentRatio = newCurrentRatio; }
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;	
};
