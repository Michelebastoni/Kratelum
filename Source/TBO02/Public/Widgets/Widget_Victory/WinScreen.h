// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameScreen.h"
#include "WinScreen.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UWinScreen : public UGameScreen
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UCustomButtonWidget> NextLevel_Widget;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<class UScoreDisplay> Score_Display_Widget;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Win_Intro_Animation;

	UPROPERTY()
	TSoftObjectPtr<UWorld> CurrentNextLevel;

	FWidgetAnimationDynamicEvent NextLevel_Event;
	FWidgetAnimationDynamicEvent Win_Intro_Event;
	
	UPROPERTY(Transient)
	TObjectPtr<class UButton> Next_Level_Button;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void Update_Customs_Buttons_Widgets() override;
	virtual void SetPrimaryWidgetButton() override;

public:
	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetWin_Intro_Animation() const { return Win_Intro_Animation; }

	UFUNCTION(BlueprintPure)
	class UScoreDisplay* GetScore_Display() const { return Score_Display_Widget; }

	UFUNCTION(BlueprintPure)
	class UCustomButtonWidget* GetNextLevel_Widget() const { return NextLevel_Widget; }

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UWorld> GetCurrentNextLevel() const { return CurrentNextLevel; }

	UFUNCTION(BlueprintCallable)
	void SetNextLevelToLoad(const TSoftObjectPtr<UWorld> newLevel) { CurrentNextLevel = newLevel; }

	UFUNCTION(BlueprintCallable)
	void StartShowScore();
private:
	UFUNCTION()
	void NotifyNext_Level_Button();

	UFUNCTION()
	void Go_To_NextLevel();

	UFUNCTION()
	void ShowScore();
};
