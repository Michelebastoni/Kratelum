#include "Widgets/Widget_Score/ScoreViewerWidget.h"
#include "Utils/SavePathNamesUtil.h"
#include "SaveSystem/ScoreSaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


void UScoreViewerWidget::LoadData(TSoftObjectPtr<UWorld> worldContext)
{
	if (worldContext.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load data into %s, due world context is null"), *GetName());
		return;
	}

	// Read file name
	const FString dataPath = SavePathNamesUtil::SCORE_DATA_FILE;

	UE_LOG(LogTemp, Log, TEXT("Searching for: %s data"), *dataPath);

	// Searching for save game data
	bool bPathExist = UGameplayStatics::DoesSaveGameExist(dataPath, 0);

	if (!bPathExist)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load %s data due file doesn't exist \nSource: [%s]"), *dataPath, *GetName());
		return;
	}

	// Load game data
	USaveGame* saveGame = UGameplayStatics::LoadGameFromSlot(dataPath, 0);

	// Cast into score save data
	UScoreSaveSystem* SavedScore = Cast<UScoreSaveSystem>(saveGame);

	if (!SavedScore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Cast saveData into ScoreSaveSystem, \nSource: [%s]"), *GetName());
		return;
	}

	FScoreData data = SavedScore->GetScoreData(worldContext);

	if(data.Score != 0)
	{
		Score_Text->SetText(FText::AsNumber(data.Score));
	}
	else
	{
		Score_Text->SetText(FText::FromString("----"));
	}

	Rank_Text->SetText(FText::FromString(data.Rank));

}

void UScoreViewerWidget::Delete_Score_Data()
{
	if (!UGameplayStatics::DoesSaveGameExist(SavePathNamesUtil::SCORE_DATA_FILE, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("File doesn't exist"));
		return;
	}

	UGameplayStatics::DeleteGameInSlot(SavePathNamesUtil::SCORE_DATA_FILE, 0);

	UE_LOG(LogTemp, Log, TEXT("File Deleted!"));
}
