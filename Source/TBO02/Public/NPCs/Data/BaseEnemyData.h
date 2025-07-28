#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseEnemyData.generated.h"

class UNiagaraSystem;
class USoundCue;

USTRUCT(BlueprintType)
struct FEnemyDropStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> Loot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	uint8 DropProbability = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector DropOffset = FVector::ZeroVector;
};

UCLASS(Abstract, BlueprintType)
class TBO02_API UBaseEnemyData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Integrity

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Health")
	int32 HealthPoints;

	// Common Movement settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Movement Settings", meta =(Units="m/s"))
	float BaseMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Movement Settings",
		meta = (ToolTip= "Expressed in m/s²"))
	float AccelerationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Movement Settings")
	float TurnAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Movement Settings")
	float MaxTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Movement Settings")
	float MinFacingTolerance;

	// Common Combat settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Combat Settings")
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Combat Settings", meta =(Units="cm"))
	float AttackRange;

	// Rank

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Rank System")
	int32 KillPoints;

	// Loot and Drops

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Loot and Drops")
	TArray<FEnemyDropStat> DropStatEntries;

	// Cosmetics

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TObjectPtr<UNiagaraSystem> SpawnVfx;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TObjectPtr<UNiagaraSystem> HitVfx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TObjectPtr<UNiagaraSystem> DeathVfx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TObjectPtr<UNiagaraSystem> TrapDeathVfx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TSoftObjectPtr<USoundCue> DeathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common Parameters|Cosmetic")
	TSoftObjectPtr<USoundCue> HitSound;
};
