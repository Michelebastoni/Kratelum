#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QTEReload.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQTEResultSignature, const UQTEReload*, reloadWidget);
/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UQTEReload : public UUserWidget
{
	GENERATED_BODY()

private:
	float ReloadRangeMultiplier = 0.0f;

public:
	UPROPERTY(BlueprintAssignable)
	FQTEResultSignature OnReloadStart;

	UPROPERTY(BlueprintAssignable)
	FQTEResultSignature OnHitOverflowZone;

	UPROPERTY(BlueprintAssignable)
	FQTEResultSignature OnNotPressed;

	UPROPERTY(BlueprintAssignable)
	FQTEResultSignature OnMissOverflowZone;

	UPROPERTY(BlueprintAssignable)
	FQTEResultSignature OnReloadEnd;

	void SetReloadRangeMultiplier(const float Value) { ReloadRangeMultiplier = Value; }

	__declspec(property(get = GetReloadRangeMultiplier, put = SetReloadRangeMultiplier)) float FReloadRangeMultiplier;

	// Ref to the slider used to interact with reload action triggered by "Generic Player Controller"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Meta = (BindWidget))
	TObjectPtr<class USlider> reloadSlider;

	UPROPERTY(EditDefaultsOnly)
	float Time_ToDisappear = 1.0f;

	// The material which the reload slider should have to accentuate the reload quick time event
	// Highlight the "Trigger Zone" where the trigger are is set up
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Material")
	TObjectPtr<class UMaterialInterface> reloadSliderMat;

	// the variable name into the reload material refered to the minValue of the trigger area
	// if there isn't this variable into the material this variable doesnt affect anithing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Material|Overflow")
	FName minOverflowValueName = "MinValue";

	// the variable name into the reload material refered to the maxValue of the trigger area
	// if there isn't this variable into the material this variable doesnt affect anithing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Material|Overflow")
	FName maxOverflowValueName = "MaxValue";

	// The minValue of the overflow reload trigger area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE|Overflow", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float overflowTriggerZoneMinValue = 0.3f;

	// The maxValue of the overflow reload trigger area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE|Overflow", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float overflowTriggerZoneMaxValue = 0.7f;

	// The offset of the reload slider on screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE|Slider")
	FVector2D reloadSliderOffset = FVector2D{ 0.0f, -100.0f };
protected:
	uint8 bCanSliderStart : 1;
	uint8 bIsPressed : 1;
	uint8 bSuccess : 1;

	UPROPERTY(Transient)
	float startTime = 0.0f;

	UPROPERTY(Transient)
	float endTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UMaterialInstanceDynamic> matInstance;

	TObjectPtr<class AGenericPlayerController> Player_Controller;

	FTimerHandle DisappearTimer;

protected:
	void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:

	// Getter
	UFUNCTION(BlueprintPure)
	class AGenericPlayerController* GetGenericPlayerController() const { return Player_Controller; }

	UFUNCTION(BlueprintPure)
	bool GetCanSliderStart() const { return bCanSliderStart; };
	
	UFUNCTION(BlueprintPure)
	bool GetIsPressed() const { return bIsPressed; };
	
	// Functions that manage reload slider logics
	UFUNCTION(BlueprintCallable)
	bool GetResult();
	
	UFUNCTION(BlueprintCallable)
	void SetOverflowTriggerZone(float minValue, float maxValue) { SetTriggerZone(overflowTriggerZoneMinValue, overflowTriggerZoneMaxValue, minValue, maxValue, minOverflowValueName, maxOverflowValueName); };
	
	UFUNCTION(BlueprintCallable)
	void SetTriggerZone(float& currentMinValue, float& currentMaxValue, float minValue, float maxValue = 1.0f, FName minValueName = "None", FName maxValueName = "None");
	
	UFUNCTION(BlueprintCallable)
	void EndReloadSlider();
	
	UFUNCTION(BlueprintCallable)
	void StartReloadSlider(float reloadDuration);
	
	UFUNCTION(BlueprintCallable)
	void UpdateSliderLocationAtLocation(FVector target);

private:
	void StartDisappearTimer();
};
