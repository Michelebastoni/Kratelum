#include "InteractionSystem/Components/InteractionComponent.h"

#include "InteractionSystem/Interfaces/Interactable.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	Interactables.Empty();
}

void UInteractionComponent::OnInteractableEntered_Implementation(const TScriptInterface<UInteractable>& Interactable)
{
	IInteractor::OnInteractableEntered_Implementation(Interactable);

	if (Interactable.GetObject())
	{
		Interactables.Add(Interactable);
	}
}

void UInteractionComponent::OnInteractableExited_Implementation(const TScriptInterface<UInteractable>& Interactable)
{
	IInteractor::OnInteractableExited_Implementation(Interactable);

	if (Interactable.GetObject())
	{
		Interactables.Remove(Interactable);
	}
}

bool UInteractionComponent::TryInteract_Implementation(const EInteractionMethod InteractionMethod,
                                                       const bool RemoveAfterInteraction)
{
	if (Interactables.IsEmpty())
	{
		return false;
	}

	if (InteractionSortingMode == EInteractionSortingMode::All)
	{

		for (int i = Interactables.Num() - 1; i >= 0; i--)
		{
			if (Interactables[i].GetObject()) 
			{
				IInteractable::Execute_Interact(Interactables[i].GetObject(), this, InteractionMethod);
			}
		}

		if (RemoveAfterInteraction)
		{
			Interactables.Empty();
		}
		return true;
	}

	TScriptInterface<UInteractable> Interactable = (InteractionSortingMode == EInteractionSortingMode::FirstOne)
												   ? Interactables[0]
												   : Interactables.Last();

	if (Interactable.GetObject())
	{
		IInteractable::Execute_Interact(Interactable.GetObject(), this, InteractionMethod);

		if (RemoveAfterInteraction)
		{
			Interactables.Remove(Interactable);
		}
		return true;
	}

	return false;
}
