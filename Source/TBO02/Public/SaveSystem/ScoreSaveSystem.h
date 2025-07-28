#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ScoreSaveSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ScoreSavingLog, Log, All);

USTRUCT(BlueprintType)
struct FScoreData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Rank = FString("-");

	UPROPERTY()
	int Score = 0;

	UPROPERTY()
	TSoftObjectPtr<UWorld> WorldContext;

	FScoreData(FString rank = FString("-"), int score = 0, TSoftObjectPtr<UWorld> worldContext = nullptr) : Rank(rank), Score(score), WorldContext(worldContext) {}
};

/**
 * 
 */
UCLASS()
class TBO02_API UScoreSaveSystem : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<FScoreData> Score_Data;

public:
	UFUNCTION(BlueprintCallable)
	void SetScoreData(class AScoreHandler* scoreHandler,	TSoftObjectPtr<UWorld> worldContext);

	FScoreData GetScoreData(TSoftObjectPtr<UWorld> worldContext);

	UFUNCTION(BlueprintCallable)
	void DeleteScoreData();
private:
	void OverrideSaveData();
};
