#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "PauseAction.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UPauseAction : public UActionBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<class UGameScreen> CurrentPauseWidget;

public:
	void SetUp(class AGenericPlayerController* controller) override;
protected:
	void Perform(const FInputActionValue& val) override;

};
