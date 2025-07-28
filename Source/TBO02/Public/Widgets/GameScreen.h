#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonFocusSignature, const class UCustomButtonWidget*, Custom_Button_Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameScreenWidgetSignature, const class UGameScreen*, GameScreen);
/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UGameScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGameScreenWidgetSignature OnGameScreenEnable;
	
	UPROPERTY(BlueprintAssignable)
	FGameScreenWidgetSignature OnGameScreenDisable;
	
	UPROPERTY(BlueprintAssignable)
	FButtonFocusSignature OnButtonFocusRecived;
	
	UPROPERTY(BlueprintAssignable)
	FButtonFocusSignature OnButtonFocusLost;

protected:
	// ***********************  VARIABLES ************************
	UPROPERTY(EditDefaultsOnly, Category = "Game Screen")
	TSoftObjectPtr<UWorld> HUB_Level;
	// ***********************************************************


	// ************************ WIDGET **************************
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UCustomButtonWidget> HUB_Widget;

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UCustomButtonWidget> Retry_Widget;
	// ***********************************************************


	// ************************ TEXTBLOCK ************************
	UPROPERTY(Transient)
	TObjectPtr<class UTextBlock> HUB_Text;

	UPROPERTY(Transient)
	TObjectPtr<class UTextBlock> Retry_Text;
	// ***********************************************************


	// ************************ WIDGET ANIMATION *****************
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Transition_Animation;
	// ***********************************************************


	uint8 bIsGameScreenActive : 1;

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<class ULoadingScreen> Loading_Screen;
	
	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<class UButton> HUB_Button;

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<class UButton> Retry_Button;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<class UCustomButtonWidget> Current_Custom_Button_Focused;

	TObjectPtr<class AGenericPlayerController> CurrentPlayerController;


private:
	FWidgetAnimationDynamicEvent Retry_Event;
	FWidgetAnimationDynamicEvent HUB_Event;
	

public:
	UFUNCTION(BlueprintPure)
	class UCustomButtonWidget* GetHUB_Widget() const { return HUB_Widget; }

	UFUNCTION(BlueprintPure)
	class UCustomButtonWidget* GetRetry_Widget() const { return Retry_Widget; }

	UFUNCTION(BlueprintPure)
	class UButton* GetHUB_Button() const { return HUB_Button; }

	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetTransition_Animation() const { return Transition_Animation; }

	UFUNCTION(BlueprintPure)
	class UButton* GetRetry_Button() const { return Retry_Button; }

	UFUNCTION(BlueprintCallable)
	void ToggleGameScreen(bool bEnable);

	bool GetIsGameScreenActive() const { return bIsGameScreenActive; }
	
	void ToggleControllerSettings(bool bEnable);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void SetPrimaryWidgetButton();
	virtual void SetGameScreenContext();
	virtual void Update_Customs_Buttons_Widgets();

	void SetPrimaryButtonFocus();
	
	UFUNCTION()
	void HandleCustomButtonFocused(class UCustomButtonWidget* widgetToCheck);

	UFUNCTION()
	void HandleCustomButtonUnfocused(class UCustomButtonWidget* widgetToCheck);

	void Update_Custom_Button_Focus(class UCustomButtonWidget* widgetToCheck);

	UFUNCTION()
	virtual void NotifyHUB_Button();

	UFUNCTION()
	virtual void NotifyRetry_Button();

private:
	UFUNCTION()
	void ReloadLevel();

	UFUNCTION()
	void Return_To_HUB();

	UFUNCTION()
	void HandleInputs(const FKey& Key, bool isGamepadInput);
};
