#include "NPCs/Subsystems/NPCSubsystem.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "VFXSystem/Actors/EnemiesVfxMultiPool.h"

void UNPCSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PatrolPoints.Empty();
	UE_LOG(LogTemp, Display, TEXT("NPC Subsystem initialized!"));
}

UNPCSubsystem* UNPCSubsystem::GetInstance(const UWorld* World)
{
	if (World)
	{
		return World->GetSubsystem<UNPCSubsystem>();
	}
	return nullptr;
}

void UNPCSubsystem::SetPlayer(APlayerCharacter* Value)
{
	Player = Value;
	OnPlayerSpawned.Broadcast(Value);
}

void UNPCSubsystem::SetVfxPool(AEnemiesVfxMultiPool* Value)
{
	EnemiesVfxMultiPool = Value;
}

void UNPCSubsystem::RegisterPatrolPoint(APatrolPoint* PatrolPoint)
{
	PatrolPoints.Add(PatrolPoint);
}

void UNPCSubsystem::AddDropToExclusionList(const TSubclassOf<AActor>& Drop)
{
	if (!Drop)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot add Null Drop nullo to exclusion list!"));
		return;
	}

	LootDropExclusionList.Add(Drop);
}

void UNPCSubsystem::RemoveDropToExclusionList(const TSubclassOf<AActor>& Drop)
{
	if (!Drop)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot remove Null Drop nullo to exclusion list!"));
		return;
	}
	
	LootDropExclusionList.Remove(Drop);
}

void UNPCSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PatrolPoints.Empty();
	UE_LOG(LogTemp, Display, TEXT("NPC Subsystem de-initialized!"));
}
