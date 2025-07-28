// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FunctionLibrary.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "NPCs/Characters/NpcBase.h"


TSubclassOf<UObject> UFunctionLibrary::WeightedExtraction(TMap<TSubclassOf<UObject>, float> EntriesWeight)
{
	float TotalWeight = 0.0f;
	for (const TPair<TSubclassOf<UObject>, float>& Entry : EntriesWeight)
	{
		TotalWeight += Entry.Value;
	}

	if (TotalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("TotalWeight <= 0."));
		return TSubclassOf<UObject>();
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	float AccWeight = 0.0f;
	for (const TPair<TSubclassOf<UObject>, float>& Entry : EntriesWeight)
	{
		AccWeight += Entry.Value;
		if (AccWeight >= RandomWeight)
		{
			return Entry.Key;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("TotalWeight error."));
	return TSubclassOf<UObject>();
}

void UFunctionLibrary::LaserTrace(USceneComponent* Origin, UNiagaraComponent* NS_Laser,
                                  UNiagaraComponent* NS_LaserImpact, float& LaserLength, float LaserExtendSpeed, float MaxLaserLength, float Damage,
                                  bool bSkipPlayer, bool bSkipEnemies)
{
	// ---- Laser Trace ----
	FCollisionQueryParams TraceParams;
	TraceParams.TraceTag = "LaserTrace";
	TraceParams.AddIgnoredActor(Origin->GetOwner());

	FHitResult HitResult(ForceInit);

	FVector StartLocation = Origin->GetComponentLocation();
	FVector EndLocation = StartLocation + Origin->GetForwardVector() * LaserLength;
	
	UWorld* World = Origin->GetWorld();
	World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
	//World->DebugDrawTraceTag = TraceParams.TraceTag;
	DrawDebugBox(World, HitResult.Location, FVector(100.0f,100.0f,100.0f), FColor::Red);

	bool bHasHit = HitResult.bBlockingHit;

	// ---- Update Beam ----
	FVector HitLocation = bHasHit ? HitResult.Location : EndLocation;
	NS_Laser->SetVectorParameter("BeamEnd", HitLocation);
	NS_LaserImpact->SetVisibility(bHasHit);
	NS_LaserImpact->SetWorldLocation(HitLocation);

	// ---- Damage ----
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel5);
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(20.0f);
	TraceParams = FCollisionQueryParams();
	World->SweepMultiByObjectType(HitResults, StartLocation, HitLocation, FQuat::Identity, ObjectQueryParams,
	                              CollisionShape, TraceParams);
	
	for(FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if(HitActor->IsA(APlayerCharacter::StaticClass()) && bSkipPlayer)
			continue;

		if(HitActor->IsA(ANpcBase::StaticClass()) && bSkipEnemies)
			continue;
		
		if (HitActor->Implements<UIntegrityHolder>())
		{
			IIntegrityHolder* IntegrityInterface = Cast<IIntegrityHolder>(HitActor);
			IntegrityInterface->Execute_Damage(HitActor, Damage, Origin->GetOwner());
		}
	}

	// ---- Update Laser Length ----
	LaserLength += World->GetDeltaSeconds() * LaserExtendSpeed;
	if(LaserLength > MaxLaserLength)
		LaserLength = MaxLaserLength;
}
