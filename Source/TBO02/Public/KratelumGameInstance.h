#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KratelumGameInstance.generated.h"

UCLASS()
class TBO02_API UKratelumGameInstance : public UGameInstance
{
	GENERATED_BODY()
	virtual void Init() override;
};
