#include "NPCs/Characters/LaserCallerNpc.h"

#include "CharacterLogic/PlayerCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "NPCs/Controllers/NpcControllerBase.h"
#include "NPCs/Data/LaserCallerEnemyData.h"
#include "Traps/Trap.h"

struct FStreamableManager;

void ALaserCallerNpc::SetupFromConfig()
{
	Super::SetupFromConfig();

	if (const ULaserCallerEnemyData* Data = Cast<ULaserCallerEnemyData>(EnemyData))
	{
		checkf(Data->OrbitalLaser, TEXT("How can a Laser Caller NPC call an Orbital Laser if it was not set?"))

		TimeToWaitBeforeCall = Data->TimeToWaitBeforeLaserCall;
		TimeToWaitAfterCall = Data->TimeToWaitAfterLaserCall;

		if (!Data->OrbitalLaser)
		{
			return;
		}

		TArray<FSoftObjectPath> SfxPaths;

		if (!Data->CallLaserAnimMontage.IsNull())
		{
			SfxPaths.Add(Data->CallLaserAnimMontage.ToSoftObjectPath());
		}

		if (SfxPaths.Num() < 0) return;

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		Streamable.RequestAsyncLoad(
			SfxPaths, FStreamableDelegate::CreateUObject(this, &ALaserCallerNpc::OnCosmeticsLoaded));
	}
}

void ALaserCallerNpc::Attack(AActor* Target)
{
	Super::Attack(Target);

	PlayAnimMontage(CallLaserAnimMontage.Get());
}

void ALaserCallerNpc::CallLaser()
{
	if (UAnimInstance* AnimInstance = MeshPtr->GetAnimInstance(); AnimInstance->Montage_IsPlaying(CallLaserAnimMontage.Get()))
	{
		AnimInstance->Montage_Stop(0.15f, CallLaserAnimMontage.Get());
	}
	
	if (!OrbitalLaser)
	{
		if (UWorld* World = GetWorld())
		{
			const ULaserCallerEnemyData* Data = Cast<ULaserCallerEnemyData>(EnemyData);
			
			OrbitalLaser = World->SpawnActor<ATrap>(Data->OrbitalLaser,
			                                        NpcController->GetCurrentPlayerTarget()->GetActorLocation(),
			                                        FRotator::ZeroRotator);
		}
	}
	OrbitalLaser->ToggleTrap();
}

void ALaserCallerNpc::HandleCharacterDeath(const AActor* character, const AActor* harmer,
                                           const class UIntegrity* component, const float currentIntegrity)
{
	if (UAnimInstance* AnimInstance = MeshPtr->GetAnimInstance(); AnimInstance->Montage_IsPlaying(CallLaserAnimMontage.Get()))
	{
		AnimInstance->Montage_Stop(0.15f, CallLaserAnimMontage.Get());
	}
	
	Super::HandleCharacterDeath(character, harmer, component, currentIntegrity);
}


void ALaserCallerNpc::OnCosmeticsLoaded()
{
	Super::OnCosmeticsLoaded();
	const ULaserCallerEnemyData* Data = CastChecked<ULaserCallerEnemyData>(EnemyData);
	CallLaserAnimMontage = Data->CallLaserAnimMontage.Get();
}

void ALaserCallerNpc::OnObjectPooled_Implementation()
{
	Super::OnObjectPooled_Implementation();
	
	OnEnemyPooledDelegate.Broadcast(true);
}

void ALaserCallerNpc::OnObjectReturnToPool_Implementation()
{
	Super::OnObjectReturnToPool_Implementation();
	
	OnEnemyPooledDelegate.Broadcast(false);
}