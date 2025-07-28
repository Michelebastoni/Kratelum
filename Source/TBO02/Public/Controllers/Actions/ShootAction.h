#pragma once

#include "CoreMinimal.h"
#include "Controllers/Actions/ActionBase.h"
#include "ShootAction.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UShootAction : public UActionBase
{
	GENERATED_BODY()
	

protected:
	void Perform(const FInputActionValue& val) override;
};
