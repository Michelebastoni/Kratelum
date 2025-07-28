#pragma once

#include "CoreMinimal.h"
#include "DamageTypeEnum.generated.h"

UENUM(BlueprintType, meta = (ScriptName = "DmgType"))
enum class EDamageType : uint8
{
    Base UMETA(DisplayName = "Base"),
    Melee UMETA(DisplayName = "Melee"),
    Ranged UMETA(DisplayName = "Ranged")
};
