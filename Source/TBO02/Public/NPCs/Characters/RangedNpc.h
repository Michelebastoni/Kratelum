#pragma once

#include "CoreMinimal.h"
#include "NPCs/Characters/NpcBase.h"
#include "RangedNpc.generated.h"

UCLASS(Abstract)
class TBO02_API ARangedNpc : public ANpcBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float weaponRandomDeviation = 5.0f;
	
public:
	virtual void Attack(AActor* Target) override;
	UFUNCTION(BlueprintCallable)
	void Reload();
	UFUNCTION(BlueprintCallable)
	bool CanReload();
};
