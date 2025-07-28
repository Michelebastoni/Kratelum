#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "MovementAction.generated.h"


struct FMovementData
{
	float WalkSpeed = 0;
};
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TBO02_API UMovementAction : public UActionBase
{
	GENERATED_BODY()
	
public:
	// Value that indicate the maxSpeed which the character can move in "Default mode" (walking)
	UPROPERTY(EditDefaultsOnly, Category = "Action", meta = (UIMin = 0.0f, Units = "CentimetersPerSecond"))
	float walkSpeed = 500.0f;

private:
	FMovementData MovementData;

public:
	void SetUp(class AGenericPlayerController* controller) override;

	UFUNCTION(BlueprintCallable)
	void SetNewSpeed(const float NewSpeed);

protected:
	void Perform(const FInputActionValue& val) override;


};
