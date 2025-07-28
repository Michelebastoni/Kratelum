#include "VFXSystem/Actors/EnemiesVfxMultiPool.h"

#include "NPCs/Subsystems/NPCSubsystem.h"
#include "PoolingSystem/Actors/Pool.h"
#include "VFXSystem/Actors/NiagaraPooledActor.h"

AEnemiesVfxMultiPool::AEnemiesVfxMultiPool()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AEnemiesVfxMultiPool::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	for (int32 i = 0; i < Pools.Num(); i++)
	{
		Pools[i]->Init();

		const TSubclassOf<AActor> Obj = Pools[i]->GetObjectToSpawn();

		if (const ANiagaraPooledActor* PooledVfx = Obj->GetDefaultObject<ANiagaraPooledActor>())
		{
			UNiagaraSystem* Sys = PooledVfx->GetVfxToSpawn();
			PoolsMap.Add(Sys, Pools[i]);
		}
	}

	if (UNPCSubsystem* NpcSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NpcSubsystem->SetVfxPool(this);
	}
}

void AEnemiesVfxMultiPool::TrySpawnVfx(const UNiagaraSystem* VfxToSpawn, const FVector& SpawnLocation)
{
	if (!PoolsMap.Contains(VfxToSpawn))
	{
		UE_LOG(LogPool, Error, TEXT("VFX not found"));
		return;
	}

	if (ANiagaraPooledActor* PooledVfx = Cast<ANiagaraPooledActor>(PoolsMap[VfxToSpawn]->TryGetPoolable()))
	{
		PooledVfx->SetActorLocation(SpawnLocation);
	}
}
