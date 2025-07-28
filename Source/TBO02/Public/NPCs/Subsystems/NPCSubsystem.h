#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NPCSubsystem.generated.h"

class AEnemiesVfxMultiPool;
class APatrolPoint;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSpawnedDelegate, APlayerCharacter*, Player);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawnedDelegate, const AActor*, Enemy);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyDiedDelegate, const AActor*, KilledEnemy, const int, KillPoints,
                                               const AActor*, Killer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopAllEnemiesDelegate);

UCLASS()
class TBO02_API UNPCSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	TWeakObjectPtr<APlayerCharacter> Player;

	TWeakObjectPtr<AEnemiesVfxMultiPool> EnemiesVfxMultiPool;

	TArray<TObjectPtr<APatrolPoint>> PatrolPoints;

	TSet<TSubclassOf<AActor>> LootDropExclusionList;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	static UNPCSubsystem* GetInstance(const UWorld* World);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSpawnedDelegate OnPlayerSpawned;
	UPROPERTY(BlueprintAssignable)
	FOnEnemySpawnedDelegate OnEnemySpawned;
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDiedDelegate OnEnemyDied;
	UPROPERTY(BlueprintAssignable)
	FOnStopAllEnemiesDelegate OnStopAllEnemies;

	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GetPlayer() const { return Player.Get(); };

	AEnemiesVfxMultiPool* GetVfxPool() const { return EnemiesVfxMultiPool.Get(); };

	UFUNCTION(BlueprintCallable)
	TSet<TSubclassOf<AActor>> GetLootDropExclusionList() const { return LootDropExclusionList; };

	void SetPlayer(APlayerCharacter* Value);

	void SetVfxPool(AEnemiesVfxMultiPool* Value);

	void RegisterPatrolPoint(APatrolPoint* PatrolPoint);

	const TArray<TObjectPtr<APatrolPoint>>& GetPatrolPoints() const { return PatrolPoints; };

	UFUNCTION(BlueprintCallable)
	void AddDropToExclusionList(const TSubclassOf<AActor>& Drop);

	UFUNCTION(BlueprintCallable)
	void RemoveDropToExclusionList(const TSubclassOf<AActor>& Drop);

	UFUNCTION(BlueprintCallable)
	void StopAllEnemies() const { OnStopAllEnemies.Broadcast(); };
};
