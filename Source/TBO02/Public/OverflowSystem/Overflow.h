#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Overflow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChangeOverflowSignature, UOverflow*, overflowComp, const float, nextOverflowValue, const float, overflowRate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TBO02_API UOverflow : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Overflow")
	FChangeOverflowSignature OnChangeOverflow;

	UPROPERTY(BlueprintAssignable, Category = "Overflow")
	FChangeOverflowSignature OnMaxOverflowReach;

	UPROPERTY(BlueprintAssignable, Category = "Overflow")
	FChangeOverflowSignature OnOverflowValueReach;

	UPROPERTY(BlueprintAssignable, Category = "Overflow")
	FChangeOverflowSignature OnOverflowAdded;

	UPROPERTY(BlueprintAssignable, Category = "Overflow")
	FChangeOverflowSignature OnOverflowRemoved;

protected:
	// The max value that the overflow value can reach
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Overflow")
	float maxOveflowValue = 100.0f;

	// The min value that the overflow value can reach
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Overflow")
	float minOveflowValue = 0.0f;

	// Indicate the overflow value at the begin play
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Overflow")
	float initialOverflowValue = 0.0f;

	// The current overflow value that will been clamped in min/max value
	UPROPERTY(VisibleAnywhere, Category = "Overflow")
	float currentOverflowValue = 0.0f;

	// The amount of time which the current overflow update his value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Overflow")
	float smoothChangeValueTime = 1.0f;

	// Indicate the sound that has to be triggered when overflow value changed
	UPROPERTY(EditDefaultsOnly, Category = "Overflow")
	TObjectPtr<USoundBase> changeOverflowSound;

	// Indicate the sound that has to be triggered when overflow value reach max value
	UPROPERTY(EditDefaultsOnly, Category = "Overflow")
	TObjectPtr<USoundBase> maxOverflowSound;
	
	// Indicate the VFX that has to be triggered when overflow value reach max value
	UPROPERTY(EditDefaultsOnly, Category = "Overflow")
	TObjectPtr<class UNiagaraSystem> maxOverflowVFX;

	// Indicate the next value that the current value should has, usefull to make the interpolation
	UPROPERTY(Transient)
	float nextOverflowValue;

	// Indiacte the previous value that the current value had, usefull to make the interpolation
	UPROPERTY(Transient)
	float prevCurrentFlow;

	// Interpolation settings
	UPROPERTY(Transient)
	float startTime;
	
	UPROPERTY(Transient)
	float endTime;
	
	UPROPERTY(Transient)
	float elapsedTime;

	// Indicate if current value has been updated with his last change
	uint8 bIsCurrentValueUpdating : 1;

	//Indicate if overflow can change his value
	uint8 bCanOverflowChange : 1;

	uint8 bAlready_Make_Sound : 1;

	uint8 bIsValueAdded : 1;

public:	
	// Sets default values for this component's properties
	UOverflow();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ------------------------ Getter -----------------------------------
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool GetCanOverflowChange() const { return bCanOverflowChange; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAlreadyMakeSound() const { return bAlready_Make_Sound; }

	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetMaxOverflowValue() const { return maxOveflowValue; };
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetMinOverflowValue() const { return minOveflowValue; };
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetCurrentOverflowValue() const { return currentOverflowValue; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentOverflowRate() const { return (currentOverflowValue - minOveflowValue) / (maxOveflowValue - minOveflowValue); };
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSmoothChangeValueTime() const { return smoothChangeValueTime; }
	
	// -------------------------------------------------------------------


	UFUNCTION(BlueprintCallable)
	void SetCanOverflowChange(bool newValue) { bCanOverflowChange = newValue; }
	
	UFUNCTION(BlueprintCallable)
	void AddOverflow(float value) { bIsValueAdded = true; ChangeOverflowValue(value); };

	void AddOverflowWithoutSound(float value) { bIsValueAdded = true; ChangeOverflowValue(value, false, false); };
	
	UFUNCTION(BlueprintCallable)
	void RemoveOverflow(float value) { bIsValueAdded = false; ChangeOverflowValue(-value); };
	
	UFUNCTION(BlueprintCallable)
	void SetOverflowValue(float newValue) { ChangeOverflowValue(newValue, true); };
	
	UFUNCTION(BlueprintCallable)
	void ResetOverflow() { SetOverflowValue(initialOverflowValue); };

protected:
	void ChangeOverflowValue(float newValue, bool set = false, bool bTriggerSound = true);
	void CheckOverflowStatus();	
};
