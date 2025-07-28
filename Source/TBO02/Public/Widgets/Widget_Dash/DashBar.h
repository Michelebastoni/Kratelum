#pragma once

#include "CoreMinimal.h"
#include "Widgets/GenericProgressBar.h"
#include "DashBar.generated.h"

UCLASS(Abstract)
class TBO02_API UDashBar : public UGenericProgressBar
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (UIMin = 0.01f, UIMax = 0.999f))
	float lowDashRatio = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	FLinearColor dashColorEmpty = FLinearColor::White;
	FLinearColor defaulDashColor = FLinearColor::Blue;
	
protected:
	virtual void NativeConstruct() override;
	
	// Methods
public:
	virtual void CheckProgressBarStatus() override;
	
	UFUNCTION(BlueprintCallable, Category = "Dash")
	void StartToRefill();
	
	UFUNCTION(Category = "Dash")
	float GetCurrentProgressBarPercent() const { return currentProgressBarPercent; }
};
