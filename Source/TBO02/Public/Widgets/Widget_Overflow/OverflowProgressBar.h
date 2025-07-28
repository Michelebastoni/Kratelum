#pragma once

#include "CoreMinimal.h"
#include "Widgets/GenericProgressBar.h"
#include "OverflowProgressBar.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UOverflowProgressBar : public UGenericProgressBar
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	TObjectPtr<class APlayerCharacter> Player;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	UFUNCTION()
	void UpdateOverflowPercent(class UOverflow* overflow, const float nextOverflowValue, const float overflowRate);

};
