#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NpcControllerBase.generated.h"

class ANpcBase;

class APlayerCharacter;

UCLASS(Abstract)
class TBO02_API ANpcControllerBase : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> Player;

	UPROPERTY(EditAnywhere, Category = "NPCs")
	FName TargetKey = "Target";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCs")
	UBehaviorTree* BehaviorTree;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ANpcControllerBase();

	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GetCurrentPlayerTarget() const { return Player.Get(); };

	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	virtual void OnUnPossess() override;

	UFUNCTION()
	virtual void SetPlayerTarget(APlayerCharacter* Target);

	UFUNCTION()
	void ResetAI();

	UFUNCTION()
	void OnNpcDied(const AActor* KilledPawn, int KillPoints, const AActor* Killer);

	void ToggleBrain(bool bValue) const;
};
