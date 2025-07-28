#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "CharacterLogic/GenericCharacter.h"
#include "NPCs/Data/BaseEnemyData.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "PoolingSystem/Interfaces/Poolable.h"
#include "NpcBase.generated.h"

class AEnemiesVfxMultiPool;
struct FEnemyDropStat;

class UNiagaraComponent;
class UBaseEnemyData;
class ANpcControllerBase;
class UPoolData;
class USoundCue;
class APool;

UCLASS(Abstract)
class TBO02_API ANpcBase : public AGenericCharacter, public IPoolable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowAbstract = "false"))
	TObjectPtr<UBaseEnemyData> EnemyData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	bool bForceAutoSetupData;

	UPROPERTY(EditAnywhere, Category="NPC|Pools")
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> InitCollisionChannelResponses;

	UPROPERTY(EditAnywhere, Category="NPC|Pools")
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> ActiveCollisionChannelResponses;

	UPROPERTY(EditAnywhere, Category="NPC|Pools")
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> InactiveCollisionChannelResponses;

	UPROPERTY(EditAnywhere, Category="NPC|Pools",
		meta = (ToolTip = "How many seconds to wait before returning this NPC to the pool."))
	float TimeToReturnToPool = 1.0f;

	UPROPERTY(EditAnywhere, Category="NPC|Pools")
	bool bDrawPoolStatusDebug = false;

	// Runtime data cache
	int32 PointsPerKill;
	float BaseMovementSpeed;
	float TimeBetweenAttacks;
	float AttackRangeSqr;
	float TurnAcceleration;
	float MovementAcceleration;
	float MaxTurnRate;
	float MinFacingTolerance;

	// Rotation cache
	FRotator CurrentRotationSpeed;

	// Pooling utils
	bool bIsPooled;

	UPROPERTY(Transient)
	TObjectPtr<ANpcControllerBase> NpcController;

	FTimerHandle PoolTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<USoundCue> HitSoundCue;
	UPROPERTY(Transient)
	TObjectPtr<USoundCue> DeathSoundCue;

	UPROPERTY(Transient)
	TArray<FEnemyDropStat> LootTable;
	
	UPROPERTY(Transient)
	UCharacterMovementComponent* MovCompPtr;
	
	UPROPERTY(Transient)
	UNPCSubsystem* NpcSubsystem;

	UPROPERTY(Transient)
	APool* Pool;

	UPROPERTY(Transient)
	AEnemiesVfxMultiPool* VfxPool;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanDrop = true;
	
	virtual void BeginPlay() override;

public:
	ANpcBase();

	UNiagaraSystem* GetSpawnVfxPtr() const { return EnemyData->SpawnVfx.Get(); }

	UNiagaraSystem* GetHitVfxPtr() const { return EnemyData->HitVfx.Get(); }

	UNiagaraSystem* GetDeathVfxPtr() const { return EnemyData->DeathVfx.Get(); }

	UNiagaraSystem* GetTrapDeathVfxPtr() const { return EnemyData->TrapDeathVfx.Get(); }

	int32 GetPointsPerKill() const { return PointsPerKill; }

	void SetPointsPerKill(const int32 Value) { this->PointsPerKill = Value; }

	float GetBaseMovementSpeed() const { return BaseMovementSpeed; }

	void SetBaseMovementSpeed(const float Value) { this->BaseMovementSpeed = Value; }

	float GetSqrAttackRange() const { return AttackRangeSqr; }

	float GetFacingTolerance() const { return MinFacingTolerance; }

	/**
	 * Sets new attack range (Value will be squared for efficiency purposes)
	 * @param Value The distance expressed in centimeters
	 */
	void SetAttackRange(const float Value) { this->AttackRangeSqr = Value * Value; }

	UFUNCTION(BlueprintCallable)
	void DropLoot();

	virtual void ToggleAI(bool bValue);

	virtual void Spawn(const FVector& SpawnPos);

	virtual void OnCosmeticsLoaded();

	virtual void SpawnDeathVfx(const AActor* Killer);

	UFUNCTION(BlueprintCallable)
	virtual void SetupFromConfig();

	UFUNCTION(BlueprintCallable)
	virtual void Attack(AActor* Target);

	virtual void UpdateRotation(float DeltaTime, const AActor* Target);

	UFUNCTION(BlueprintCallable)
	virtual void ResetNpcState();

	virtual void HandleCharacterDeath(const AActor* character, const AActor* harmer, const class UIntegrity* component,
	                                  const float currentIntegrity) override;

	virtual void HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue,
	                                     const float currentIntegrityValue, const float changedValue,
	                                     const float integrityValueRate, AActor* invader) override;

	UFUNCTION()
	virtual void ToggleCapsuleCollision();
	
	// Pooling system
	UFUNCTION()
	void AutoReturnToPool();
	virtual void Initialize_Implementation(class APool* PoolValue) override;
	virtual void OnObjectPooled_Implementation() override;
	virtual void OnObjectReturnToPool_Implementation() override;
	virtual bool GetIsInUse_Implementation() override { return bIsPooled; }
	
	bool TryCacheVfxPool();

private:
	UFUNCTION()
	void OnEnemyHasToStop();
};
