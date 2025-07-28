#pragma once

#include "CoreMinimal.h"
#include "Widgets/GenericProgressBar.h"
#include "IntegirtyWidget.generated.h"

UCLASS()
class TBO02_API UIntegrityWidget : public UGenericProgressBar
{
	GENERATED_BODY()
	
private:
	TObjectPtr<class UIntegrity> Integrity;
	UPROPERTY(EditAnywhere, Category = "Integrity", meta = (UIMin = 0.01f, UIMax = 0.999f))
	float lowHpRatio = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Integrity")
	FLinearColor lowHpColor = FLinearColor::Red;
	FLinearColor defaultHpColor = FLinearColor::White;
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual void CheckProgressBarStatus() override;

private:
	UFUNCTION()
	void OnHealthChanged(const UIntegrity* integrityComp, const float previousIntegrityValue, const float  currentIntegrityValue, const float changedValue, const float integrityValueRate, AActor* invader);
	
};
