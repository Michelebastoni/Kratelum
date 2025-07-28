#pragma once

#include "CoreMinimal.h"
#include "CharacterLogic/GenericCharacter.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInvincibilitySignature, const APlayerCharacter*, Player);
/**
 * 
 */
UCLASS()
class TBO02_API APlayerCharacter : public AGenericCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FInvincibilitySignature OnStartInvicibility;
	
	UPROPERTY(BlueprintAssignable)
	FInvincibilitySignature OnEndInvicibility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class USpringArmComponent> springArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class UCameraComponent> camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class UOverflow> overflowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class UDecalComponent> MagazineRingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Invicibility")
	float invicibilityTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Material")
	TObjectPtr<class UMaterialInterface> MagazineRingMat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Material")
	FName MagazineRingPercentName;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> MagazineRing;

	UPROPERTY()
	TObjectPtr<class AGenericPlayerController> Player_Controller;

	FTimerHandle invicibilityTimer;
	FTimerHandle InfiniteDashTimer;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
public:
	APlayerCharacter();
	virtual void HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue, const float  currentIntegrityValue, const float changedValue, const float integrityValueRate, AActor* invader) override;
	

	virtual void ChangeWeaponParameters(class UWeaponParametersDataAsset* newWeaponAsset, float weaponDuration, bool bIsWeaponBase) override;

	UFUNCTION(BlueprintPure)
	class AGenericPlayerController* GetGenericController() const { return Player_Controller; }

	UFUNCTION(BlueprintPure)
	class UMaterialInstanceDynamic* GetMagazineRingMat() const { return MagazineRing; }

	UFUNCTION(BlueprintPure)
	class UCameraComponent* GetCameraComponent() const { return camera; }
	
	UFUNCTION(BlueprintPure)
	class UOverflow* GetOverflowComponent() const { return overflowComponent; }
	
	UFUNCTION()
	void RotateCharacter(const FVector& deltaRotation);
	
	UFUNCTION()
	void OnInvincibilityEnd();

	UFUNCTION()
	void HandleMagazineDecal(const int bullets);

	UFUNCTION(BlueprintCallable)
	void EnableInfiniteDash(float Duration);

	UFUNCTION(BlueprintCallable)
	void TogglePersistentInfiniteDash(bool Enable);
	
private:
	UFUNCTION()
	void DisableInfiniteDash();
};
