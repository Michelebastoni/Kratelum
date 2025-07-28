#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionSystem/Interfaces/Interactable.h"
#include "InteractableActor.generated.h"

class UInteractor;
class IInteractor;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractorEnterDelegate, const TScriptInterface<UInteractor>&,
                                             Interactor, const TScriptInterface<UInteractable>&, Interactable);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractorExitDelegate, const TScriptInterface<UInteractor>&,
                                             Interactor, const TScriptInterface<UInteractable>&, Interactable);

/**
 * Class from which every interactable object should inherit. \n
 * Provides delegates to notify whenever something that can interact with it enters/exits its interaction trigger.
 */
UCLASS(Abstract)
class TBO02_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction System")
	class USphereComponent* InteractionTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction System")
	EInteractionMethod InteractionType;

public:
	AInteractableActor();

	/**
	 * How the interaction must be performed?
	 * @return Interaction should be performed via KeyPress, Shooting or Both?
	 */
	EInteractionMethod GetInteractionMethod() const { return InteractionType; }

	/**
	 * Sets how the interaction must be performed.
	 */
	void SetInteractionMethod(const EInteractionMethod Value) { InteractionType = Value; }

	/**
	 * Triggered whenever a new Interactor enters the interaction trigger. \n
	 * Passes a reference to the Interactor that entered the trigger and a reference to itself.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnInteractorEnterDelegate OnInteractorEnterDelegate;

	/**
	 * Triggered whenever a new Interactor exits the interaction trigger. \n
	 * Passes a reference to the Interactor that exited the trigger and a reference to itself.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnInteractorExitDelegate OnInteractorExitDelegate;

protected:
	virtual void BeginPlay() override;

	/**
	 * An array containing all the Interactors in the interaction trigger.
	 */
	UPROPERTY()
	TArray<TScriptInterface<UInteractor>> Interactors;

	/**
	 * Callback for when an Interactor enters the InteractionTrigger box component.
	 * @param OverlappedComponent 
	 * @param OtherActor 
	 * @param OtherComponent 
	 * @param OtherIndex 
	 * @param bFromSweep 
	 * @param SweepResult 
	 */
	UFUNCTION()
	void OnInteractionZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComponent, int OtherIndex, bool bFromSweep,
	                              const FHitResult& SweepResult);

	/**
	 * Callback for when an Interactor exits the InteractionTrigger box component.
	 * @param OverlappedComponent 
	 * @param OtherActor 
	 * @param OtherComponent 
	 * @param OtherIndex 
	 */
	UFUNCTION()
	void OnInteractionZoneExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComponent, int OtherIndex);

public:
	/**
	 * Utility method to understand whether an Interactor can perform the desired interaction via its method.
	 * @param Interactor The Interactor requiring the interaction.
	 * @param InteractionMethod The Interactor's interaction method.
	 * @return Can the interaction be performed?
	 */
	virtual bool CanInteract_Implementation(const TScriptInterface<UInteractor>& Interactor,
	                                        EInteractionMethod InteractionMethod) override;

	/**
	 * Performs the interaction between the implementor of this interface and the Interactor via
	 * the provided interaction method.
	 * @param Interactor The Interactor requiring the interaction.
	 * @param InteractionMethod The Interactor's interaction method.
	 */
	virtual void Interact_Implementation(const TScriptInterface<UInteractor>& Interactor,
	                                     EInteractionMethod InteractionMethod) override;
};
