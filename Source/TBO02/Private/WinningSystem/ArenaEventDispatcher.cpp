#include "WinningSystem/ArenaEventDispatcher.h"

#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "SpawningSystem/Spawners/EnemySpawner.h"

void AArenaEventDispatcher::BeginPlay()
{
	Super::BeginPlay();

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.AddDynamic(this, &ThisClass::OnEnemyDied);
	}

	if (!AEnemySpawner::OnPhaseCompleted.IsBoundToObject(this))
		AEnemySpawner::OnPhaseCompleted.AddUObject(this, &AArenaEventDispatcher::OnPhaseCompleted);
}

void AArenaEventDispatcher::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.RemoveDynamic(this, &ThisClass::OnEnemyDied);
	}

	AEnemySpawner::OnPhaseCompleted.RemoveAll(this);
}

void AArenaEventDispatcher::OnEnemyDied(const AActor* KilledEnemy, const int32 PointsPerKill,
                                              const AActor* Killer)
{
	KilledEnemies++;

	// Global winning condition is met
	if (KilledEnemies >= EnemiesToKill)
	{
		OnLevelWonDelegate.Broadcast(LevelIndex);
	}
}

void AArenaEventDispatcher::OnPhaseCompleted(const UEnemyPhaseData* PreviousPhase,
                                                   const UEnemyPhaseData* CurrentPhase, const int PhaseIndex,
                                                   const bool IsLastPhase) const
{
	// Was this phase one of the trigger phases?
	if (TriggerPhases.Contains(PreviousPhase))
	{
		OnPhaseCompletedDelegate.Broadcast(PreviousPhase, CurrentPhase, PhaseIndex, IsLastPhase);
	}
}
