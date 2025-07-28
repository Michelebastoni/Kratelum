#include "NPCs/Utilities/PatrolPoint.h"

#include "NPCs/Subsystems/NPCSubsystem.h"

APatrolPoint::APatrolPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	if (UNPCSubsystem* NpcSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NpcSubsystem->RegisterPatrolPoint(this);
	}
}
