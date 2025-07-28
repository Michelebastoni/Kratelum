#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemiesVfxMultiPool.generated.h"

class UNiagaraSystem;
class APool;
class ANiagaraPooledActor;

UCLASS()
class TBO02_API AEnemiesVfxMultiPool : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TArray<APool*> Pools;

	UPROPERTY(Transient)
	TMap<UNiagaraSystem*, APool*> PoolsMap;

public:
	AEnemiesVfxMultiPool();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void TrySpawnVfx(const UNiagaraSystem* VfxToSpawn, const FVector& SpawnLocation);
};
