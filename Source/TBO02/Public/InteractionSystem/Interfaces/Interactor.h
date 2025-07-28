#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactor.generated.h"

class UInteractable;
enum class EInteractionMethod : uint8;

/**
 * Defines how an Interactor should deal with all the Interactables it's into when interacting. 
 */
UENUM(BlueprintType)
enum class EInteractionSortingMode : uint8
{
	/**
	 * Interact only with the first interactable encountered.
	 */
	FirstOne UMETA(DisplayName = "First One"),
	/**
 	 * Interact only with the last interactable encountered.
 	 */
	LastOne UMETA(DisplayName = "Last One"),
	/**
	 * Interact with all the interactables encountered.
	 */
	All UMETA(DisplayName = "All")
};

/**
 * Interface that all classes that want to perform interactions with interactables should inherit from.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that all classes that want to perform interactions with interactables should inherit from.
 */
class TBO02_API IInteractor
{
	GENERATED_BODY()

public:
	/**
	 * Callback triggered whenever entering an Interactable's trigger zone.
	 * @param Interactable The Interactable this object is entering into.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction System")
	void OnInteractableEntered(const TScriptInterface<UInteractable>& Interactable);

	/**
 	 * Callback triggered whenever exiting from an Interactable's trigger zone.
 	 * @param Interactable The Interactable this object is exiting from.
 	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction System")
	void OnInteractableExited(const TScriptInterface<UInteractable>& Interactable);

	/**
	 * Tries to interact with the Interactable(s) passing its Interaction Method. 
	 * @param InteractionMethod How the interaction should be performed? (KeyPress, Shooting, Both)
	 * @param RemoveAfterInteraction Should the Interactable(s) be removed after the interaction is performed?
	 * @return Whether the interaction(s) was/were successful.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction System")
	bool TryInteract(EInteractionMethod InteractionMethod, bool RemoveAfterInteraction = false);
};
