#include "IntegritySystem/Integrity.h"

void UIntegrity::AddImmortalitySource()
{
	ImmortalityContainer.Add(1);
}

void UIntegrity::RemoveImmortalitySource()
{
	if(ImmortalityContainer.IsEmpty())
	{
		return;
	}
	else
	{
		const int8 length = ImmortalityContainer.Num();
		ImmortalityContainer.RemoveAt(length - 1);
	}
	
}

void UIntegrity::BeginPlay()
{
	Super::BeginPlay();
	
	owner = GetOwner();
	CurrentIntegrityValue = InitialIntegrityValue;

}

UIntegrity::UIntegrity() :
	bIsAddIntegrity(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UIntegrity::ChangeIntegrity(float changeValue, AActor* harmer, bool bSetValue)
{
	// If integrity increase dont need condition 
	if(!bIsAddIntegrity)
	{
		// If there are immortality source into array, doesn't recive damage
		if (!ImmortalityContainer.IsEmpty())
		{
			return;
		}
	}

	// Set previous integrity
	PreviousIntegrityValue = CurrentIntegrityValue;

	// Set or add/subtract integrity value
	if(bSetValue)
	{
		CurrentIntegrityValue = changeValue;
	}
	else
	{
		CurrentIntegrityValue += changeValue;
	}

	CurrentIntegrityValue = FMath::Clamp(CurrentIntegrityValue, 0.0f, MaxIntegrityValue);

	// Calls events
	if(bIsAddIntegrity)
	{
		OnRestoreIntegrity.Broadcast(
			this,
			PreviousIntegrityValue,
			CurrentIntegrityValue,
			changeValue,
			GetIntegrityValueRate(),
			harmer
		);
	}
	else
	{
		OnRemoveIntegrity.Broadcast(
			this,
			PreviousIntegrityValue,
			CurrentIntegrityValue,
			changeValue,
			GetIntegrityValueRate(),
			harmer
		);
	}

	OnChangeIntegrity.Broadcast(
		this,
		PreviousIntegrityValue,
		CurrentIntegrityValue,
		changeValue,
		GetIntegrityValueRate(),
		harmer
		);

	if(CurrentIntegrityValue <= 0)
	{
		OnDeath.Broadcast(owner, harmer, this, CurrentIntegrityValue);
	}
	
}
