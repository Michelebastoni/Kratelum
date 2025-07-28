#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "DashAction.generated.h"

struct FDashData
{
	uint8 bPersistentInfinityDash : 1;

	uint8 bInfinityDash : 1;

	int MaxDashCount = 2;

	float DashCoolDown = 1.0f;

	int32 DashCount = 1;

	float DashDistance = 5000.0f;

	float DashMovethreshold = 0.01f;

	float DashCoolDownMultiplier = 1.0f;

	TObjectPtr<USoundBase> DashSFX;

	TObjectPtr<class UNiagaraSystem> DashVFX;

	TObjectPtr<USoundBase> DashReadySFX;

	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> DashCollisionSettings;

	float DashAOEDamage = 100.0f;

	float DashAOERadius = 100.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> DashObjectDetection;

	TObjectPtr<USoundBase> DashExplosionSFX;

	TObjectPtr<class UNiagaraSystem> DashExplosionVFX;
};

class UNiagaraSystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashSignature, const UDashAction*, dashAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashChangeSignature, const int, currentDashCount);

/**
 * 
 */
UCLASS()
class TBO02_API UDashAction : public UActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Action")
	FDashSignature OnDashEnd;

	UPROPERTY(BlueprintAssignable, Category = "Action")
	FOnDashChangeSignature OnDashCooldownEnd;

	UPROPERTY(BlueprintAssignable, Category = "Action")
	FDashSignature OnDashCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "Action")
	FOnDashChangeSignature OnDashPerformed;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle dashCooldownTimer;

private:
	FDashData DashData;

	// -------------------------- DASH ------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	bool bInfinityDash = false;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	int maxDashCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash", meta = (Units = "Seconds"))
	float dashCoolDown = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	float dashDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	float dashMovethreshold = 0.01f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	float dashCoolDownMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	TObjectPtr<USoundBase> dashSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	TObjectPtr<USoundBase> dashReadySFX;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	TObjectPtr<class UNiagaraSystem> dashVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Dash")
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> dashCollisionSettings;
	// --------------------------------------------------------------------------

	// --------------------------- AOE DAMAGE -----------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	bool bDrawSphereDamageDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	float dashAOEDamage = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	float dashAOERadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	TArray<TEnumAsByte<EObjectTypeQuery>> dashObjectDetection;

	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	TObjectPtr<USoundBase> dashExplosionSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Action|AOE settings")
	TObjectPtr<class UNiagaraSystem> dashExplosionVFX;
	// --------------------------------------------------------------------------

	UPROPERTY(Transient)
	uint8 bIsDashing : 1;

	UPROPERTY(Transient)
	int dashCount = 0;

	UPROPERTY(Transient)
	TObjectPtr<class UNiagaraComponent> dashVFXComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UIntegrity> IntegrityComponent;

	UPROPERTY(Transient)
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> savedDashCollisionSettings;

public:
	void SetUp(class AGenericPlayerController* controller) override;
	void ShutDown() override;

	void AddDashCount(const int dashCountAmount = 1) { SetDashCount(DashData.DashCount + dashCountAmount); }
	void RemoveDashCount(const int dashCountAmount = 1) { SetDashCount(DashData.DashCount - dashCountAmount); }

	//------------------------ Getter ------------------------------
	UFUNCTION(BlueprintPure)
	int32 GetCurrenDashCount() const { return DashData.DashCount; }

	UFUNCTION(BlueprintPure)
	int32 GetMaxDashCount() const { return DashData.MaxDashCount; }

	UFUNCTION(BlueprintPure)
	bool GetPersistentInfinityDash() const { return DashData.bPersistentInfinityDash; }

	UFUNCTION(BlueprintPure)
	float GetDashCoolDown() const { return DashData.DashCoolDown * DashData.DashCoolDownMultiplier; }

	UFUNCTION(BlueprintPure)
	float GetDashDistance() const { return DashData.DashDistance; }

	UFUNCTION(BlueprintPure)
	bool GetIsDashing() const { return bIsDashing; }

	void SetInfinityDash(bool bNewValue) { DashData.bInfinityDash = bNewValue; }

	float GetDashCoolDownTimer() const;
	// -------------------------------------------------------------


	// ----------------------- Setter ------------------------------
	UFUNCTION(BlueprintCallable)
	void SetMaxDashCount(const int newMaxValue) { DashData.MaxDashCount = newMaxValue; }

	UFUNCTION(BlueprintCallable)
	void SetDashDistance(const float newDistance) { DashData.DashDistance = newDistance; }

	UFUNCTION(BlueprintCallable)
	void SetDashCoolDownMultiplier(const float newMultplier);

	UFUNCTION(BlueprintCallable)
	void SetDashSFX(class USoundBase* NewSound) { DashData.DashSFX = NewSound; }

	UFUNCTION(BlueprintCallable)
	void SetDashReadySFX(class USoundBase* NewSound) { DashData.DashReadySFX = NewSound; }

	UFUNCTION(BlueprintCallable)
	void SetDashVFX(class UNiagaraSystem* NewVFX);

	UFUNCTION(BlueprintCallable)
	void SetDashExplosionSFX(class USoundBase* NewSound) { DashData.DashExplosionSFX = NewSound; }

	UFUNCTION(BlueprintCallable)
	void SetDashExplosionVFX(class UNiagaraSystem* NewVFX) { DashData.DashExplosionVFX = NewVFX; }

	UFUNCTION(BlueprintCallable)
	void SetDashCount(const int newDashCount);

	UFUNCTION(BlueprintCallable)
	void ResetDashCount() { DashData.DashCount = DashData.MaxDashCount; }

	UFUNCTION(BlueprintCallable)
	void ResetDashCoolDownMultplier() { DashData.DashCoolDownMultiplier = 1.0f; }

	void TogglePersistentInfinityDash(bool bEnable) { DashData.bPersistentInfinityDash = bEnable; }

	// --------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void MakeDash(const float distance);

protected:
	void Perform(const FInputActionValue& val) override;

private:
	void SetUpData();

	bool GetStrongerCondition() const;

	void ToggleDashSettings(bool enable);

	UFUNCTION()
	void OnDashCoolDownFinished();

	UFUNCTION()
	void OnDashFinished();
};
