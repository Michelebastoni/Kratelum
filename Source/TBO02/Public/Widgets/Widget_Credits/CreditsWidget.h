#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditsWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UCreditsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<class UWorld> HUB;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> Credits_Animation;

	UPROPERTY()
	TObjectPtr<class AGenericPlayerController> Player_Controller;

	FWidgetAnimationDynamicEvent Credits_Event;

protected:
	virtual void NativeConstruct();
	virtual void NativeDestruct();

public:
	UFUNCTION(BlueprintPure)
	class AGenericPlayerController* GetGenericPlayerController() const { return Player_Controller; }

	UFUNCTION(BlueprintCallable)
	void PlayCreditsAnimation();

private:
	UFUNCTION()
	void OnCreditsFinished();

	UFUNCTION()
	void SetUp(const class AGenericPlayerController* GenericPlayerController);

	UFUNCTION()
	void SkipCinematic_Notify();
};
