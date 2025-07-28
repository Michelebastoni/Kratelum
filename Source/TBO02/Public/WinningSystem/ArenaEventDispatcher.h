#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawningSystem/Data/EnemyPhaseData.h"
#include "ArenaEventDispatcher.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelWonDelegate, int, LevelIndex);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPhaseCompletedDelegate, const UEnemyPhaseData*, PreviousPhase,
                                              const UEnemyPhaseData*, CurrentPhase, const int, PhaseIndex, const bool,
                                              IsLastPhase);

/**
 * Class that monitors and triggers events whenever a given condition is met.
 * Should be used primarily by blueprints to trigger cosmetic logic.
 */
UCLASS()
class TBO02_API AArenaEventDispatcher : public AActor
{
	GENERATED_BODY()

	int KilledEnemies;

public:
	/**
	 * Delegate broadcasted when winning condition is met.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnLevelWonDelegate OnLevelWonDelegate;

	/**
	 * Delegate broadcasted whenever a spawner's phase is completed.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPhaseCompletedDelegate OnPhaseCompletedDelegate;

	/**
	 * How many enemies should be killed to broadcast winning delegate?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EnemiesToKill;

	/**
	 * The index of the current won level.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LevelIndex;

	/**
	 * Which phases should trigger the OnPhaseCompleted Delegate?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UEnemyPhaseData>> TriggerPhases;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/**
	 * Callback triggered whenever an enemy dies.\n
	 * If EnemiesToKill number is reached, it will trigger the
	 * winning condition delegate OnLevelWon.
	 * @param KilledEnemy The enemy that died.
	 * @param PointsPerKill How many points that enemy is worth.
	 * @param Killer Who killed the enemy.
	 */
	UFUNCTION()
	void OnEnemyDied(const AActor* KilledEnemy, const int32 PointsPerKill, const AActor* Killer);

	/**
	 * Callback triggered whenever a spawner's phase is completed,
	 * triggers the OnPhaseCompleted delegate if the completed phase
	 * was in the trigger phase array.
	 * @param PreviousPhase The phase that was completed.
	 * @param CurrentPhase The new phase that is currently active.
	 * @param PhaseIndex The index of the new phase.
	 * @param IsLastPhase Is the CurrentPhase the last phase?
	 */
	UFUNCTION()
	void OnPhaseCompleted(const UEnemyPhaseData* PreviousPhase, const UEnemyPhaseData* CurrentPhase, int PhaseIndex,
	                      bool IsLastPhase) const;
};
