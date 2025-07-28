#pragma once

#include "CoreMinimal.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "MeleeNpcController.generated.h"

UCLASS(Abstract)
class TBO02_API AMeleeNpcController : public ANpcControllerBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName AttackWindupTimeKey;

public:
	virtual void SetWindupTime(const float Value);
};
