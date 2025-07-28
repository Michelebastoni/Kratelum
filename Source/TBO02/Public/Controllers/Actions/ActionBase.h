#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputTriggers.h"
#include "ActionBase.generated.h"

/**
 * 
 */

UCLASS(Abstract, BlueprintType)
class TBO02_API UActionBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
protected:
	// Input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	TObjectPtr<class UInputAction> action;

	// Trigger method
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	ETriggerEvent eventTrigger = ETriggerEvent::Started;

	// Debug option
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	bool bEnableDebugMessage = false;

	// controller reference
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<class AGenericPlayerController> playerController;

	// player reference
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<class APlayerCharacter> Player;

	// Wolrd ref
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<UWorld> world;
public:
	
	UFUNCTION(BlueprintCallable)
	virtual void SetUp(class AGenericPlayerController* controller);

	UFUNCTION(BlueprintCallable)
	virtual void ShutDown();
protected:
	UFUNCTION(BlueprintCallable)
	virtual void Perform(const FInputActionValue& val);
};
