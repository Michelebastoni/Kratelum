#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IntegrityHolder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UIntegrityHolder : public UInterface
{
	GENERATED_BODY()
};

class TBO02_API IIntegrityHolder
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Integrity")
	void Damage(float damageValue, AActor* harmer);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Integrity")
	void Restore(float restoreValue, AActor* healer);
};
