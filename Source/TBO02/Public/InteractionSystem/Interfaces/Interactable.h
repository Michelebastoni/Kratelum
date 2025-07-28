#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class UInteractor;

UENUM(BlueprintType)
enum class EInteractionMethod : uint8
{
	KeyPress UMETA(DisplayName = "Key Press"),
	Shooting UMETA(DisplayName = "Shooting"),
	Both UMETA(DisplayName = "Both")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class TBO02_API IInteractable
{
	GENERATED_BODY()

public:
	/**
	 * Utility method to understand whether an Interactor can perform the desired interaction via its method.
	 * @param Interactor The Interactor requiring the interaction.
	 * @param InteractionMethod The Interactor's interaction method.
	 * @return Can the interaction be performed?
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction System")
	bool CanInteract(const TScriptInterface<UInteractor>& Interactor, EInteractionMethod InteractionMethod);

	/**
	 * Performs the interaction between the implementor of this interface and the Interactor via
	 * the provided interaction method.
	 * @param Interactor The Interactor requiring the interaction.
	 * @param InteractionMethod The Interactor's interaction method.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction System")
	void Interact(const TScriptInterface<UInteractor>& Interactor, EInteractionMethod InteractionMethod);
};
