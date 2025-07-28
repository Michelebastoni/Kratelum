#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreen.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API ULoadingScreen : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	bool bHideAtStart = true;

	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	float WaitSecondsForLoad = 2.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TArray<TSoftObjectPtr<UWorld>> Levels;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Transition_Animation;

	FWidgetAnimationDynamicEvent TranstionEvent;

	FName CurrentLevelNameToLoad;
	uint8 bIsAffectedByTimeDilation : 1;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
public:

	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetTransition_Animation() const { return Transition_Animation; }

	void SetAffectedByTimeDilation(bool newValue) { bIsAffectedByTimeDilation = newValue; }

	UFUNCTION(BlueprintCallable)
	void OpenLevelByIndex(const int32 LevelIndex);

	UFUNCTION(BlueprintCallable)
	void OpenLevelByLevel(TSoftObjectPtr<class UWorld> LevelToLoad);
	
	UFUNCTION(BlueprintCallable)
	void OpenLevelByName(FName LevelToLoad);
private:
	void StartLevelTimer();
	void OpenLevelByName();
	UFUNCTION()
	void OpenLevel();
};
