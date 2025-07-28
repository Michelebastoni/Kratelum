#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(Transient, Meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> HUD_Fade;

public:
	UFUNCTION(BlueprintCallable)
	class UWidgetAnimation* GetHUD_FadeAnimation() const { return HUD_Fade; }
	
	UFUNCTION(BlueprintCallable)
	void PlayFadeAnimation(bool bForwardAnim, float startTime);
};
