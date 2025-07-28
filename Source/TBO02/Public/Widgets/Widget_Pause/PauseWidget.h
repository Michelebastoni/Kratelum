#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameScreen.h"
#include "PauseWidget.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UPauseWidget : public UGameScreen
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCustomButtonWidget> Resume_Widget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCustomButtonWidget> Quit_Widget;
	
	UPROPERTY(Transient)
	TObjectPtr<class UButton> Resume_Button;

	UPROPERTY(Transient)
	TObjectPtr<class UButton> Quit_Button;


public:
	UFUNCTION(BlueprintPure)
	class UCustomButtonWidget* GetResume_Widget() const { return Resume_Widget; }


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void Update_Customs_Buttons_Widgets() override;
	virtual void SetPrimaryWidgetButton() override;

	virtual void SetGameScreenContext() override;
private:
	UFUNCTION()
	void NotifyResume_Button();

	UFUNCTION()
	void NotifyQuit_Button();
};
