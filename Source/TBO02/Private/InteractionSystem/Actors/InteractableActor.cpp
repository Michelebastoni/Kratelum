#include "InteractionSystem/Actors/InteractableActor.h"

#include "Components/SphereComponent.h"
#include "InteractionSystem/Interfaces/Interactor.h"

AInteractableActor::AInteractableActor()
{
	InteractionTrigger = CreateDefaultSubobject<USphereComponent>("Interaction_Trigger");
	
	SetRootComponent(InteractionTrigger);
}


void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionTrigger)
	{
		InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnInteractionZoneEntered);
		InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnInteractionZoneExited);
	}
}

void AInteractableActor::OnInteractionZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComponent, int OtherIndex, bool bFromSweep,
                                                  const FHitResult& SweepResult)
{
	// Ignore null or self compenetration
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	TScriptInterface<UInteractor> InteractorInterface = nullptr;
	TScriptInterface<UInteractable> InteractableInterface(this);

	if (OtherActor->Implements<UInteractor>())
	{
		InteractorInterface = TScriptInterface<UInteractor>(OtherActor);
	}
	else
	{
		if (UInteractor* InteractorComp = OtherActor->FindComponentByInterface<UInteractor>())
		{
			InteractorInterface = TScriptInterface<UInteractor>(InteractorComp);
		}
	}

	if (!InteractorInterface) return;

	const auto Target = InteractorInterface.GetObject();
	if (const IInteractor* InteractorPtr = Cast<IInteractor>(Target))
	{
		InteractorPtr->Execute_OnInteractableEntered(Target, InteractableInterface);
	}

	if (!Interactors.Contains(InteractorInterface))
	{
		Interactors.Add(InteractorInterface);
		OnInteractorEnterDelegate.Broadcast(InteractorInterface, InteractableInterface);
	}
}

void AInteractableActor::OnInteractionZoneExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComponent, int OtherIndex)
{
	// Ignore null or self compenetration
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	TScriptInterface<UInteractor> InteractorInterface = nullptr;
	TScriptInterface<UInteractable> InteractableInterface(this);

	if (OtherActor->Implements<UInteractor>())
	{
		InteractorInterface = TScriptInterface<UInteractor>(OtherActor);
	}
	else
	{
		if (UInteractor* InteractorComp = OtherActor->FindComponentByInterface<UInteractor>())
		{
			InteractorInterface = TScriptInterface<UInteractor>(InteractorComp);
		}
	}

	if (!InteractorInterface) return;

	const auto Target = InteractorInterface.GetObject();
	if (IInteractor* InteractorPtr = Cast<IInteractor>(Target))
	{
		InteractorPtr->Execute_OnInteractableExited(Target, InteractableInterface);
	}

	Interactors.Remove(InteractorInterface);
	OnInteractorExitDelegate.Broadcast(InteractorInterface, InteractableInterface);
}

bool AInteractableActor::CanInteract_Implementation(const TScriptInterface<UInteractor>& Interactor,
                                                    EInteractionMethod InteractionMethod)
{
	return true;
}

void AInteractableActor::Interact_Implementation(const TScriptInterface<UInteractor>& Interactor,
                                                 EInteractionMethod InteractionMethod)
{
	IInteractable* Interactable = Cast<IInteractable>(this);
	if (!Interactable->Execute_CanInteract(this, Interactor, InteractionMethod))
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot interact!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Interaction performed!"));
}
