#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFocusSignature, UCustomButtonWidget*, customWidget);

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UCustomButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FFocusSignature OnFocusRecived;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FFocusSignature OnFocusMissing;

private:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FText Button_Text;

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UButton> Button;

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text;
	
	// ************************ WIDGET ANIMATION ****************
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Button_Focus_Animation;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Button_Clicked_Animation;
	// **********************************************************



	// ************************ SFX ******************************
	UPROPERTY(EditDefaultsOnly, Category = "Settings|SFX")
	TObjectPtr<USoundBase> Focus_Button_SFX;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|SFX")
	TObjectPtr<USoundBase> Clicked_Button_SFX;
	// ***********************************************************



	// ************************ LINEAR COLORS ********************
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FLinearColor Focus_Color_ButtonBackground;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FLinearColor Default_Color_ButtonBackground;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FSlateColor Focus_Color_Text;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FSlateColor Default_Color_Text;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FLinearColor Clicked_Color_ButtonBackground;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Cosmetic")
	FSlateColor Clicked_Color_Text;
	// **********************************************************

public:
	UFUNCTION(BlueprintPure)
	class UButton* GetButton() const { return Button; }

	UFUNCTION(BlueprintPure)
	class UTextBlock* GetText() const { return Text; }

	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetFocusAnimation() const { return Button_Focus_Animation; }

	UFUNCTION(BlueprintPure)
	class UWidgetAnimation* GetClickedAnimation() const { return Button_Clicked_Animation; }

public:
	void Update_Focus_Status(bool bTriggerSound = true);
	void Toggle_Focus_Color(bool bIsSelected);
	void PlayFocusAnimation(bool bReverse);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnFocus_Notify();

	UFUNCTION()
	void OnUnfocus_Notify();

	UFUNCTION()
	void OnClicked_Notify();

	void PlayFocusSound();
	void PlayClickedSound();
};
