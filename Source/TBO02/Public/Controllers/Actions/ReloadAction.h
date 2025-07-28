#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "ReloadAction.generated.h"


struct FReloadData
{
	UPROPERTY()
	TObjectPtr<USoundBase> Success_Reload_SFX;

	UPROPERTY()
	TObjectPtr<USoundBase> Start_Reload_SFX;

	UPROPERTY()
	TObjectPtr<USoundBase> Failed_Reload_SFX;

	float PerfectReloadOveflowValue = 1.0f;
	
	float ReloadDelay = 0.3f;
};
/**
 * 
 */
UCLASS()
class TBO02_API UReloadAction : public UActionBase
{
	GENERATED_BODY()
	
private:
	FReloadData ReloadData;

	// Input release reload action
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<class UInputAction> reloadReleaseAction;

	// release Trigger method
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	ETriggerEvent releaseEventTrigger = ETriggerEvent::Started;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Success_Reload_SFX;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Start_Reload_SFX;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Failed_Reload_SFX;

	UPROPERTY(Transient)
	TObjectPtr<class UQTEReload> reloadWidget;

	UPROPERTY(EditDefaultsOnly)
	float perfectReloadOveflowValue = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float reloadDelay = 0.3f;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage>currentReloadMontage;

	FTimerHandle reloadTimer;
	uint8 bSound_Already_Play : 1;

public:
	void SetUp(class AGenericPlayerController* controller) override;
	void ShutDown() override;

protected:
	void Perform(const FInputActionValue& val) override;
	
	UFUNCTION()
	void OnReloadSuccess(const class UQTEReload* reloadQTEWidget);
	
	UFUNCTION()
	void OnReloadFailed(const class UQTEReload* reloadQTEWidget);

	UFUNCTION()
	void OnReloadEnd(const class UQTEReload* reloadQTEWidget);
private:
	UFUNCTION()
	void PerformReload();
	UFUNCTION()
	void ReleaseReload();
	
	void SetUpData();
};
