#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkipText.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API USkipText : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkipText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> SkipText_Animation;

	UPROPERTY()
	TObjectPtr<class AGenericPlayerController> Player_Controller;

	FWidgetAnimationDynamicEvent SkipAnimation_Event;
	uint8 bCanAnimationBegin : 1;
public:
	UFUNCTION(BlueprintPure)
	class UTextBlock* GetSkipText() const { return SkipText; }

	UFUNCTION(BlueprintPure)
	bool GetCanAnimationBegin() const { return bCanAnimationBegin; }
protected:
	virtual void NativeConstruct();
	virtual void NativeDestruct();

	UFUNCTION(BlueprintCallable)
	void PlaySkipAnimation(bool bForward = true, float timeToStart = 0.0f);

	UFUNCTION(BlueprintNativeEvent)
	void ChangeText(bool isGamePadInput);
private:
	UFUNCTION()
	void OnInputRecived_Notify(const FKey& Key, bool isGamepadInput);
	
	UFUNCTION()
	void MakeSkipAnimationPlayable() { bCanAnimationBegin = true; };
};
