#include "NPCs/Controllers/NpcControllerBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "NPCs/Characters/NpcBase.h"
#include "NPCs/Subsystems/NPCSubsystem.h"

ANpcControllerBase::ANpcControllerBase(): Player(nullptr), BehaviorTree(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANpcControllerBase::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GetWorld();

	if (!World) return;
	
	UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld());

	if (!NPCSubsystem) return;

	if (APlayerCharacter* PlayerPtr = NPCSubsystem->GetPlayer())
	{
		Player = PlayerPtr;
	}
	else
	{
		NPCSubsystem->OnPlayerSpawned.AddDynamic(this, &ThisClass::SetPlayerTarget);
	}
	
	NPCSubsystem->OnEnemyDied.AddDynamic(this, &ThisClass::OnNpcDied);
}

void ANpcControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTree)
	{
		UE_LOG(LogTemp, Error, TEXT("Behaviour Tree is null"));
		return;
	}

	UBlackboardComponent* BlackboardComponent = nullptr;
	if (!UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize Blackboard"));
		return;
	}

	Blackboard = BlackboardComponent;

	if (!Blackboard)
	{
		UE_LOG(LogTemp, Error, TEXT("Blackboard is null"));
		return;
	}

	Cast<ANpcBase>(GetPawn())->SetupFromConfig();

	RunBehaviorTree(BehaviorTree);
	
	if (IsValid(Player))
	{		
		Blackboard->SetValueAsObject(TargetKey, Player.Get());
	}
}

void ANpcControllerBase::OnUnPossess()
{
	Super::OnUnPossess();
}

void ANpcControllerBase::ToggleBrain(const bool bValue) const
{
	UBrainComponent* Brain = GetBrainComponent();

	if (!Brain) return;

	if (bValue)
	{
		Brain->ResumeLogic(FString("Resuming NPC brain!"));
	}
	else
	{
		Brain->PauseLogic(FString("Pausing NPC brain!"));
	}
}

void ANpcControllerBase::ResetAI()
{
	ANpcBase* Npc = Cast<ANpcBase>(GetPawn());
	if (!Npc)
		return;

	// For safety only
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}

	Npc->ResetNpcState();
}

void ANpcControllerBase::SetPlayerTarget(APlayerCharacter* Target)
{
	Player = Target;

	if (Blackboard)
	{
		Blackboard->SetValueAsObject(TargetKey, Target);
	}
}

void ANpcControllerBase::OnNpcDied(const AActor* KilledPawn, int KillPoints, const AActor* Killer)
{
	if (KilledPawn != GetPawn()) return;

	StopMovement();
}

void ANpcControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnPlayerSpawned.RemoveDynamic(this, &ThisClass::SetPlayerTarget);
		NPCSubsystem->OnEnemyDied.RemoveDynamic(this, &ThisClass::OnNpcDied);
	}
}
