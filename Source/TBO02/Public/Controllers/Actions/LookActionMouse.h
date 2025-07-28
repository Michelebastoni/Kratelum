#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "LookActionMouse.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API ULookActionMouse : public UActionBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float RotationSensitivity = 2.0f;

protected:
	void Perform(const FInputActionValue& val) override;
};
