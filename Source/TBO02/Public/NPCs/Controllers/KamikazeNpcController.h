#pragma once

#include "CoreMinimal.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "KamikazeNpcController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetonationValueChanged, bool, bIsDetonating);

UCLASS()
class TBO02_API AKamikazeNpcController : public ANpcControllerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnDetonationValueChanged OnDetonationValueChangedDelegate;

protected:
	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName AttackWindupTimeKey;

	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName IsDetonatingKey;

	virtual void BeginPlay() override;

public:
	virtual void SetWindupTime(const float Value);

	void SetIsDetonating(const bool bValue) const;
	bool GetIsDetonating() const;
};
