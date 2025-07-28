#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "LookAction.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API ULookAction : public UActionBase
{
	GENERATED_BODY()

protected:
	void Perform(const FInputActionValue& val) override;
};
