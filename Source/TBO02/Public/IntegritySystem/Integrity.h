#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Integrity.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FIntegritySignature, const UIntegrity*, integrityComp, const float, previousIntegrityValue, const float, currentIntegrityValue,const float, changedValue, const float, integrityValueRate, AActor*, harmer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDeathSignature, const AActor*, owner, const AActor*, harmer, const UIntegrity*, integrityComp, const float, currentIntegrityValue);

/*
The name used is quite clear, but if you still don't understand what this component is for, read below.
"Integrity" is an Actor Component that is used to monitor the integrity of the assigned actor.
It may decrease or increase itself, but not more than the maximum indicated.
It also grants immunity to all or some damage and the recovery block.
From this Script 3 events start: 1- provide integrity info. 
2/3- provide info about itself and its owner.
*/
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TBO02_API UIntegrity : public UActorComponent
{
	GENERATED_BODY()

	// fields
public:
	UPROPERTY(BlueprintAssignable, Category = "Integrity")
	FIntegritySignature OnChangeIntegrity;

	UPROPERTY(BlueprintAssignable, Category = "Integrity")
	FIntegritySignature OnRestoreIntegrity;

	UPROPERTY(BlueprintAssignable, Category = "Integrity")
	FIntegritySignature OnRemoveIntegrity;

	UPROPERTY(BlueprintAssignable, Category = "Integrity")
	FDeathSignature OnDeath;

private:
	UPROPERTY(EditAnywhere, Category = "Integrity")
	bool bInvincible = false;

	UPROPERTY(EditAnywhere, Category = "Integrity", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float MaxIntegrityValue = 100.0f;
	
	UPROPERTY(EditAnywhere, Category = "Integrity", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float InitialIntegrityValue = 100.0f;


	TObjectPtr<AActor> owner;
	TArray<uint8> ImmortalityContainer;
	float CurrentIntegrityValue;
	float PreviousIntegrityValue;
	uint8 bIsAddIntegrity : 1;

	//methods
public:
	// ---------------------------- Getter --------------------------------------
	float GetInitialIntegrityValue() const { return InitialIntegrityValue; }
	
	UFUNCTION(BlueprintPure, Category = "Integrity")
	TArray<uint8> GetImmortalityContainer() const { return ImmortalityContainer; }

	UFUNCTION(BlueprintPure, Category = "Integrity")
	float GetMaxIntegrityValue() const { return MaxIntegrityValue; }
	
	UFUNCTION(BlueprintPure, Category = "Integrity")
	float GetCurrentIntegrityValue() const { return CurrentIntegrityValue; }

	UFUNCTION(BlueprintPure, Category = "Integrity")
	float GetPreviousIntegrityValue() const { return PreviousIntegrityValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Integrity")
	float GetIntegrityValueRate() const { return CurrentIntegrityValue / MaxIntegrityValue; };
	
	// --------------------------------------------------------------------------


	// ---------------------------- Setter --------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void AddImmortalitySource();

	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void RemoveImmortalitySource();

	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void RemoveAllImmortalitySource() { ImmortalityContainer.Empty(); }

	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void SetMaxIntegrityValue(float newValue) { MaxIntegrityValue = newValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void SetIntegrityValue(float newValue, AActor* harmer) { ChangeIntegrity(newValue, harmer, true); }
	// --------------------------------------------------------------------------

	//Remove integrity function
	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void SubtractIntegrityValue(float subValue, AActor* harmer = nullptr) { bIsAddIntegrity = false; ChangeIntegrity(-subValue, harmer); };

	//Add Integrity functions
	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void AddIntegrityValue(float addValue, AActor* harmer) { bIsAddIntegrity = true; ChangeIntegrity(addValue, harmer); };
	
	UFUNCTION(BlueprintCallable, Category = "Integrity")
	void FullRestore() { SetIntegrityValue(MaxIntegrityValue, nullptr); }

protected:
	virtual void BeginPlay() override;
	UIntegrity();
private:
	void ChangeIntegrity(float changeValue, AActor* harmer, bool bSetValue = false);
};
