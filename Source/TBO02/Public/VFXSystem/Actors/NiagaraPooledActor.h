#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolingSystem/Interfaces/Poolable.h"
#include "NiagaraPooledActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class TBO02_API ANiagaraPooledActor : public AActor, public IPoolable
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(Transient)
	APool* NiagaraPool;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> Vfx;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 1.0f;

public:
	ANiagaraPooledActor();
	
	UNiagaraSystem* GetVfxToSpawn() const { return Vfx.Get(); }

	virtual bool GetIsInUse_Implementation() override;
	virtual void Initialize_Implementation(APool* Pool) override;
	virtual void OnObjectPooled_Implementation() override;
	virtual void OnObjectReturnToPool_Implementation() override;

	UFUNCTION()
	void OnEndInitVfx();
	UFUNCTION()
	void OnEndVfx();
};
