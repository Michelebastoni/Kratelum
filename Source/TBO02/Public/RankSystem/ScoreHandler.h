#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScoreHandler.generated.h"

UENUM(BlueprintType)
enum class ERank : uint8
{
	K	UMETA(DisplayName = "K Rank"),
	S	UMETA(DisplayName = "S Rank"),
	A	UMETA(DisplayName = "A Rank"),
	B	UMETA(DisplayName = "B Rank"),
	C	UMETA(DisplayName = "C Rank"),
	D	UMETA(DisplayName = "D Rank"),
	None UMETA(DisplayName = "NaN")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FScoreTrapKillSignature, const AScoreHandler*, owner, bool, bIsTrapKill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreSignature, const AScoreHandler*, owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRankSiganture, const enum ERank, currentRank);


UCLASS()
class TBO02_API AScoreHandler : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Score Settings")
	FScoreSignature OnScoreComboStarted;

	UPROPERTY(BlueprintAssignable, Category = "Score Settings")
	FRankSiganture OnRankUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Score Settings")
	FScoreTrapKillSignature OnScoreUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Score Settings")
	FScoreTrapKillSignature OnScoreComboEnd;

private:
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	bool bEnableDebugMessage = true;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	TSubclassOf<class UScoreWidget> scoreWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Score Settings")
	int initialScore = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	float initialScoreMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	float comboTime = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Score Settings")
	float multiplierCap = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	float multiplierDelta = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	float trapMultiplier = 2.0f;
	
	UPROPERTY(EditAnywhere, Category = "Score Settings")
	TMap<ERank, int> rankSettings;

	UPROPERTY()
	ERank CurrentRank = ERank::None;

	UPROPERTY(Transient)
	float CurrentMultiplier = 0.0f;
	
	UPROPERTY(Transient)
	float CurrentTrapMultiplier = 0.0f;
	
	UPROPERTY()
	int CurrentScore = 0;
	
	UPROPERTY(Transient)
	int CurrentKillPoint = 0;
	
	UPROPERTY(Transient)
	TObjectPtr<class APlayerCharacter> Player;
	
	UPROPERTY(Transient)
	TObjectPtr<class UScoreWidget> ScoreWidget;

	UPROPERTY()
	FTimerHandle ComboTimer;

	UPROPERTY(Transient)
	int PreviousScore = 0;

	UPROPERTY(Transient)
	uint8 bIsComboStarted : 1;
	
	UPROPERTY(Transient)
	uint8 bIsTrapKill : 1;

public:
	AScoreHandler();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ------------------------ Getter --------------------------
	UFUNCTION(BlueprintPure)
	class UScoreWidget* GetScoreWidget() const { return ScoreWidget; }
	
	UFUNCTION(BlueprintPure)
	bool GetIsTrapKill() const { return bIsTrapKill; }
	
	UFUNCTION(BlueprintPure)
	ERank GetCurrentRank() const { return CurrentRank; }

	UFUNCTION(BlueprintPure)
	FString GetCurrentRankAsString() const;
	
	UFUNCTION(BlueprintPure)
	ERank GetRankFromScore(const int score) const;

	UFUNCTION(BlueprintPure)
	float GetCurrentScoreMultiplier() const { return CurrentMultiplier; }
	
	UFUNCTION(BlueprintPure)
	float GetCurrentTrapMultiplier() const { return CurrentTrapMultiplier; }
	
	UFUNCTION(BlueprintPure)
	int GetPreviousScore() const { return PreviousScore; }
	
	UFUNCTION(BlueprintPure)
	int GetCurrentScore() const { return CurrentScore; }
	
	UFUNCTION(BlueprintPure)
	int GetCurrentKillPoint() const { return CurrentKillPoint; }
	
	UFUNCTION(BlueprintPure)
	FTimerHandle GetComboTimer() const { return ComboTimer; }
	
	UFUNCTION(BlueprintPure)
	float GetComboTimeRate() const;	
	// ----------------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void ResetScoreMultiplier();
	
	UFUNCTION(BlueprintCallable)
	void ResetScore() { CurrentScore = initialScore; };
	
	UFUNCTION(BlueprintCallable)
	void SetMultiplierCap(float newCap) { multiplierCap = newCap; }
	
	UFUNCTION(BlueprintCallable)
	void IncreaseScore(int scoreToAdd);
	
	UFUNCTION(BlueprintCallable)
	void IncreaseScoreMultiplier();
	
	UFUNCTION(BlueprintCallable)
	void HandleScore(const AActor* killedActor,const int pointValue, const AActor* harmer);
private:
	void HandleComboTimer(bool restart);
	void UpdateRank() { CurrentRank = GetRankFromScore(CurrentScore); }
#if WITH_EDITOR
	void GetInfo();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void CheckForAnotherScoreHandler();
	void ShowEditorNotification(const FString& Message);
#endif // WITH_EDITOR

};
