#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSystem/Interfaces/Interactor.h"
#include "InteractionComponent.generated.h"

class IInteractable;

/**
 * Component that can be attached to objects to perform interactions.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TBO02_API UInteractionComponent : public UActorComponent, public IInteractor
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	/**
	 * The Array of all the Interactables the actor is into.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Interaction System")
	TArray<TScriptInterface<UInteractable>> Interactables;

	/**
	 * How the interactions should be performed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction System")
	EInteractionSortingMode InteractionSortingMode;

	virtual void BeginPlay() override;

public:
	/**
 	 * Callback triggered whenever entering an Interactable's trigger zone.
 	 * @param Interactable The Interactable this object is entering into.
 	 */
	virtual void OnInteractableEntered_Implementation(const TScriptInterface<UInteractable>& Interactable) override;

	/**
  	 * Callback triggered whenever exiting from an Interactable's trigger zone.
  	 * @param Interactable The Interactable this object is exiting from.
  	 */
	virtual void OnInteractableExited_Implementation(const TScriptInterface<UInteractable>& Interactable) override;

	/**
 	 * Tries to interact with the Interactable(s) passing its Interaction Method. 
 	 * @param InteractionMethod How the interaction should be performed? (KeyPress, Shooting, Both)
 	 * @param RemoveAfterInteraction Should the Interactable(s) be removed after the interaction is performed?
 	 * @return Whether the interaction(s) was/were successful.
 	 */
	virtual bool TryInteract_Implementation(const EInteractionMethod InteractionMethod, const bool RemoveAfterInteraction = false) override;
};