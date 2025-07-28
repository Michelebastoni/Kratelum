#include "NPCs/Utilities/DamageSphere.h"

#include "CharacterLogic/PlayerCharacter.h"


UDamageSphere::UDamageSphere(): SphereRadius(0), SphereDamage(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageSphere::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	
	SetGenerateOverlapEvents(false);

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnComponentBeginOverlap.AddDynamic(this, &UDamageSphere::TryDamageActor);
}

void UDamageSphere::Init(const float Radius, const float Damage)
{
	SphereRadius = Radius;
	SphereDamage = Damage;
	SetSphereRadius(SphereRadius);
}

void UDamageSphere::Toggle(const bool Value)
{
	if (Value)
	{
		SetGenerateOverlapEvents(true);
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		SetGenerateOverlapEvents(false);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UDamageSphere::TryDamageActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	const AActor* CompOwner = GetOwner();

	// null or itself
	if (!OtherActor || OtherActor == CompOwner)
	{
		return;
	}

	// Has an integrity component? Isn't a player? Should always pass but who knows...
	if (!OtherActor->Implements<UIntegrityHolder>() || !Cast<APlayerCharacter>(OtherActor))
	{
		return;
	}

	// Finally... Time to shine
	if (const IIntegrityHolder* Player = Cast<IIntegrityHolder>(OtherActor))
	{
		Player->Execute_Damage(OtherActor, SphereDamage, GetOwner());
	}
}
