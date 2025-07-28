#include "OverflowSystem/Overflow.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values for this component's properties
UOverflow::UOverflow() : nextOverflowValue(0), prevCurrentFlow(0), startTime(0), endTime(0), elapsedTime(0),
                         bCanOverflowChange(true),
                         bAlready_Make_Sound(0),
                         bIsValueAdded(0),
                         bIsCurrentValueUpdating(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UOverflow::BeginPlay()
{
	Super::BeginPlay();
	currentOverflowValue = initialOverflowValue;
}


// Called every frame
void UOverflow::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the interpolation time is not started yet return
	if (endTime < GetWorld()->GetTimeSeconds())
	{
		// If the overflow value is been changed and the value has not reached the max value, update it and check for event
		if(bIsCurrentValueUpdating)
		{
			// Make sure the current value is the next overflow value
			currentOverflowValue = nextOverflowValue;

			// Toggle bool to stop update tick
			bIsCurrentValueUpdating = false;

			// Check if need to call event
			CheckOverflowStatus();

			SetComponentTickEnabled(false);

			OnOverflowValueReach.Broadcast(this, currentOverflowValue, GetCurrentOverflowRate());
		}
		return;
	}

	// Interpolation of the overflow value
	currentOverflowValue = FMath::GetMappedRangeValueClamped(
		FVector2D{ startTime, endTime },
		FVector2D{ prevCurrentFlow, nextOverflowValue },
		GetWorld()->GetTimeSeconds()
	);

}


/// <summary>
/// This function has the purpose to change the actual overflow value
/// trigger the interpolation condition and call change event
/// </summary>
/// <param name="value"> indicate the value to add/subtract to the current overflow value</param>
void UOverflow::ChangeOverflowValue(float newValue, bool set, bool bTriggerSound)
{
	if (!bCanOverflowChange)
	{
		UE_LOG(LogTemp, Display, TEXT("Overflow cannot change!"));
		return;
	}

	// If the current value is changing during another change value call,
	// set the current value to the next value
	if (bIsCurrentValueUpdating)	
	{
		currentOverflowValue = nextOverflowValue;
	}

	bIsCurrentValueUpdating = true;

	// Save the previous overflow value
	prevCurrentFlow = currentOverflowValue;
 
	if (!set)
	{
		nextOverflowValue = prevCurrentFlow + newValue;	
	}
	else
	{
		nextOverflowValue = newValue;
	}

	nextOverflowValue = FMath::Clamp(nextOverflowValue, minOveflowValue, maxOveflowValue); //-> clamp the new value

	// Play sound
	if(bTriggerSound && changeOverflowSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), changeOverflowSound);
	}

	// If already on max value stop here and call method
	if (nextOverflowValue >= maxOveflowValue && currentOverflowValue >= maxOveflowValue)
	{
		CheckOverflowStatus();
		return;
	}

	// Setup the start/end time to trigger interpolation of the overflow value
	startTime = GetWorld()->GetTimeSeconds();
	endTime = startTime + smoothChangeValueTime;

	const float overflowRate = (nextOverflowValue - minOveflowValue) / (maxOveflowValue - minOveflowValue);
	
	SetComponentTickEnabled(true);

	// Call event
	OnChangeOverflow.Broadcast(this, nextOverflowValue, overflowRate);

}

// This function has the purpose to check the current value and, if the value reached the max value, call the event, otherwise make bool true to update it
void UOverflow::CheckOverflowStatus()
{
	// If current overflow value is already max call event
	if (currentOverflowValue >= maxOveflowValue)
	{
		// Play Sound
		if(maxOverflowSound && !bAlready_Make_Sound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), maxOverflowSound);
			bAlready_Make_Sound = true;
		}
		
		// Play VFX
		if(maxOverflowVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				maxOverflowVFX,
				GetOwner()->GetActorLocation()
			);
		}

		const float overflowRate = (nextOverflowValue - minOveflowValue) / (maxOveflowValue - minOveflowValue);
		
		if(bIsValueAdded)
		{
			OnOverflowAdded.Broadcast(this, currentOverflowValue, overflowRate);
		}
		else
		{
			OnOverflowRemoved.Broadcast(this, currentOverflowValue, overflowRate);
		}

		// Call event
		OnMaxOverflowReach.Broadcast(this, currentOverflowValue, overflowRate);
	}
	else
	{
		bAlready_Make_Sound = false;
	}
}
