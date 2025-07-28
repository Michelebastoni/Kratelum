#pragma once

#include "CoreMinimal.h"
#include "Widgets/GenericProgressBar.h"
#include "WeaponTimeBar.generated.h"

UCLASS()
class TBO02_API UWeaponTimeBar : public UGenericProgressBar
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (UIMin = 0.01f, UIMax = 0.999f))
	float lowWeaponRatio = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FLinearColor lowWeaponColor = FLinearColor::Red;
	FLinearColor defaultWeaponColor = FLinearColor::White; // si autoimposta allo start
	
	UPROPERTY()
	TObjectPtr<class AGenericCharacter> GenericCharacter;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
public:
	void SetUp(class AGenericCharacter* characterRef);
	virtual void CheckProgressBarStatus() override;

private:
	UFUNCTION()
	void OnPickUpNewWeapon(class AWeapon* newWeapon, const float weaponDuration, bool bIsBaseWeapon);
	
};

