// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionSystem/Components/InteractableSphere.h"

#include "InteractionSystem/Interfaces/Interactor.h"

void UInteractableSphere::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UInteractableSphere::OnInteractionZoneEntered);
	OnComponentEndOverlap.AddDynamic(this, &UInteractableSphere::OnInteractionZoneExited);
	
	if (DisableCollisionAtStart)
	{
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UInteractableSphere::OnInteractionZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore null or self compenetration
	if (!OtherActor || OtherActor == GetOwner())
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

void UInteractableSphere::OnInteractionZoneExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherIndex)
{
	// Ignore null or self compenetration
	if (!OtherActor || OtherActor == GetOwner())
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

bool UInteractableSphere::CanInteract_Implementation(const TScriptInterface<UInteractor>& Interactor,
	EInteractionMethod InteractionMethod)
{
	return true;
}

void UInteractableSphere::Interact_Implementation(const TScriptInterface<UInteractor>& Interactor,
	EInteractionMethod InteractionMethod)
{
	IInteractable* Interactable = Cast<IInteractable>(this);
	if (!Interactable->Execute_CanInteract(this, Interactor, InteractionMethod))
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot interact!"));
		return;
	}

	OnInteractionPerformed.Broadcast(Interactor, this);
	UE_LOG(LogTemp, Log, TEXT("Interaction performed!"));
}
