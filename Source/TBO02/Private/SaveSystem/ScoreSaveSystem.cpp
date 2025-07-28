#include "SaveSystem/ScoreSaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/SavePathNamesUtil.h"
#include "RankSystem/ScoreHandler.h"

DEFINE_LOG_CATEGORY(ScoreSavingLog);

void UScoreSaveSystem::SetScoreData(AScoreHandler* scoreHandler, TSoftObjectPtr<UWorld> worldContext)
{
	if(!scoreHandler)
	{
		UE_LOG(ScoreSavingLog, Warning, TEXT("Cannot find score handler to Set score data \nSource: [%s]"), *GetName());
		return;
	}

	if(worldContext.IsNull())
	{
		UE_LOG(ScoreSavingLog, Warning, TEXT("Cannot find worldContext to Set score data \nSource: [%s]"), *GetName());
		return;
	}

	FScoreData data = FScoreData(
		scoreHandler->GetCurrentRankAsString(),
		scoreHandler->GetCurrentScore(),
		worldContext
	);

	Score_Data.Add(data);

	OverrideSaveData();

	UE_LOG(ScoreSavingLog, Log, TEXT("Saved data: \nRank:[%s] \nScore: [%d] \nWorldContext: [%s] \nSource: [%s]"),
		*data.Rank,
		 data.Score,
		*data.WorldContext.GetLongPackageName(),
		*GetName()
	);

}

FScoreData UScoreSaveSystem::GetScoreData(TSoftObjectPtr<UWorld> worldContext)
{
	if(worldContext.IsNull())
	{
		UE_LOG(ScoreSavingLog, Warning, TEXT("Failed to get score data due worldContext is null \nSource: [%s]"), *GetName());
		return FScoreData();
	}

	for(int32 i = 0; i < Score_Data.Num(); i++)
	{
		UE_LOG(ScoreSavingLog, Log, TEXT("Compared %s to %s"),
			*Score_Data[i].WorldContext.GetLongPackageName(),
			*worldContext.GetLongPackageName()
		);


		// Find the right data with worldContext as index
		if(Score_Data[i].WorldContext == worldContext)
		{
			
			UE_LOG(ScoreSavingLog, Log, TEXT("Found ScoreData:\nRank: [%s] \nScore: [%d] \nWorldContext: [%s]"),
				*Score_Data[i].Rank,
				Score_Data[i].Score,
				*worldContext.GetLongPackageName()
			);

			return Score_Data[i];
		}

	}

	UE_LOG(ScoreSavingLog, Warning, TEXT("Failed to get score data from %s world context \nSource: [%s]"), *worldContext.GetLongPackageName(), *GetName());

	return FScoreData();
}

void UScoreSaveSystem::DeleteScoreData()
{
	if (!UGameplayStatics::DoesSaveGameExist(SavePathNamesUtil::SCORE_DATA_FILE, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("File doesn't exist"));
		return;
	}

	UGameplayStatics::DeleteGameInSlot(SavePathNamesUtil::SCORE_DATA_FILE, 0);

	UE_LOG(LogTemp, Log, TEXT("File Deleted!"));
}

void UScoreSaveSystem::OverrideSaveData()
{
	// Check if file data exist
	if (UGameplayStatics::DoesSaveGameExist(SavePathNamesUtil::SCORE_DATA_FILE, 0))
	{
		UScoreSaveSystem* saved = Cast<UScoreSaveSystem>(UGameplayStatics::LoadGameFromSlot(SavePathNamesUtil::SCORE_DATA_FILE, 0));

		// Read previous saving
		for (int32 i = 0; i < saved->Score_Data.Num(); i++)
		{
			// If index of current array is out of bound and theres a previous data that as to be added into array
			if (i >= Score_Data.Num())
			{
				Score_Data.Add(saved->Score_Data[i]);
				continue;
			}

			// If is the same worldcontext and the previous score is higher override the data
			if (saved->Score_Data[i].WorldContext == Score_Data[i].WorldContext)
			{
				if(Score_Data[i].Score < saved->Score_Data[i].Score)
				{
					Score_Data[i] = saved->Score_Data[i];
				}
			}
			else // -> otherwise, add data into score array
			{
				Score_Data.Add(saved->Score_Data[i]);
			}
		}
	}

	// Save data
	UGameplayStatics::SaveGameToSlot(this, SavePathNamesUtil::SCORE_DATA_FILE, 0);
}
