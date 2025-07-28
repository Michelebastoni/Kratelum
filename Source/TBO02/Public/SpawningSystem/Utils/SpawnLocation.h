#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SpawnLocation.generated.h"

/**
 * Utility class that represent an empty point in the spawning and enemy system.\n
 * Used to auto filter assets selection and minimize errors,
 * enforcing consistency of the related systems.
 */
UCLASS(MinimalAPI)
class ASpawnLocation : public ATargetPoint
{
	GENERATED_UCLASS_BODY()
};
