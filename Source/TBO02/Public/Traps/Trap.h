#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

UCLASS(Abstract)
class TBO02_API ATrap : public AActor
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	bool bOverrideTrapMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (EditCondition = bOverrideTrapMultiplier))
	float TrapMultiplier;
	
	ATrap();

	// Functions 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ToggleTrap();
	virtual void ToggleTrap_Implementation();

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Config")
	bool bShouldDamagePlayer = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, category = "State")
	bool bIsActive = false;
};
