#include "Weapons/MeleeComponent.h"

#include "ShaderPrintParameters.h"
#include "IntegritySystem/Integrity.h"
#include "Perception/AISense_Damage.h"


AActor* UMeleeComponent::Melee(UWorld* world, FVector targetPosition)
{
	if (!meleeHolder) UE_LOG(LogTemp, Warning, TEXT("%s's MeleeHolder is null"), *GetOwner()->GetActorNameOrLabel());

	FHitResult hit;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(meleeHolder);

	FVector startPos = GetOwner()->GetActorLocation();
	FVector direction = targetPosition - startPos;
	direction.Normalize();

	if (world->LineTraceSingleByChannel(hit, startPos, startPos + direction * meleeRange, ECC_Camera, queryParams))
	{
		if (!hit.GetActor())
			return nullptr;

		if(hit.GetActor()->Implements<UIntegrityHolder>())
		{
			IIntegrityHolder* integrityInterface = Cast<IIntegrityHolder>(hit.GetActor());

			integrityInterface->Execute_Damage(hit.GetActor(), meleeDamage, meleeHolder);
		}

		if (AGenericCharacter* character = Cast<AGenericCharacter>(hit.GetActor()))
		{
			character->ApplyImpact();
			
		}
		else
		{
			return nullptr;
		}

		//TODELETE
		UE_LOG(LogTemp, Log, TEXT("Melee hit [%s]"), *hit.GetActor()->GetActorNameOrLabel())
		return hit.GetActor();
	}

	return nullptr;
}

void UMeleeComponent::ApplyDamageMultiplier(const float MultiplierValue)
{
	DamageMultiplier = MultiplierValue;
}

void UMeleeComponent::ResetDamageMultiplier()
{
	DamageMultiplier = 1.0f;
}
