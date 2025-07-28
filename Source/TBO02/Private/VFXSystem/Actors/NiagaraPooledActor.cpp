#include "VFXSystem/Actors/NiagaraPooledActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "PoolingSystem/Actors/Pool.h"


ANiagaraPooledActor::ANiagaraPooledActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	RootComponent = NiagaraComponent;

	NiagaraComponent->bAutoActivate = false;
}

bool ANiagaraPooledActor::GetIsInUse_Implementation()
{
	return TimerHandle.IsValid();
}

void ANiagaraPooledActor::Initialize_Implementation(APool* Pool)
{
	if (!Vfx)
	{
		UE_LOG(LogPool, Error, TEXT("No Vfx asset set on [%s]"), *GetName())
		return;
	}

	if (Duration <= 0.0f)
	{
		UE_LOG(LogPool, Error, TEXT("Vfx duration is equal or below zero, effect won't play on [%s]"), *GetName())
		return;
	}

	NiagaraPool = Pool;

	NiagaraComponent->SetAsset(Vfx.Get());
	NiagaraComponent->InitializeSystem();

	NiagaraComponent->ActivateSystem();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANiagaraPooledActor::OnEndInitVfx, 0.25);

	SetActorHiddenInGame(true);
}

void ANiagaraPooledActor::OnObjectPooled_Implementation()
{
	UE_LOG(LogPool, Display, TEXT("VFX [%s] is required from pool!"), *GetName())
	SetActorHiddenInGame(false);
	NiagaraComponent->ActivateSystem();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANiagaraPooledActor::OnEndVfx, Duration);
}

void ANiagaraPooledActor::OnObjectReturnToPool_Implementation()
{
	UE_LOG(LogPool, Display, TEXT("VFX [%s] returning to pool!"), *GetName())
	SetActorHiddenInGame(true);
	NiagaraComponent->DeactivateImmediate();
}

void ANiagaraPooledActor::OnEndInitVfx()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	NiagaraComponent->DeactivateImmediate();
}

void ANiagaraPooledActor::OnEndVfx()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	NiagaraPool->ReturnPoolable(this);
}
