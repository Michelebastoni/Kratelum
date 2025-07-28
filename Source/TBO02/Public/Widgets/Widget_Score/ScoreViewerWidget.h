#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreViewerWidget.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Score_Text;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Rank_Text;


public:
	UFUNCTION(BlueprintCallable)
	void LoadData(TSoftObjectPtr<UWorld> worldContext);
	
	UFUNCTION(BlueprintCallable)
	void Delete_Score_Data();
};
