#include "Widgets/Widget_Loading/LoadingScreen.h"
#include "Kismet/GameplayStatics.h"

void ULoadingScreen::NativeConstruct()  
{
	Super::NativeConstruct();

	if(bHideAtStart)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	bIsAffectedByTimeDilation = false;

	TranstionEvent.BindDynamic(this, &ThisClass::OpenLevel);
}

void ULoadingScreen::NativeDestruct()
{
	Super::NativeDestruct();

	TranstionEvent.Unbind();
	if(TranstionEvent.IsBoundToObject(this))
	{
		UnbindFromAnimationFinished(Transition_Animation, TranstionEvent);
	}
}

void ULoadingScreen::OpenLevelByIndex(const int32 LevelIndex)
{
	if(LevelIndex < 0 || LevelIndex >= Levels.Num() || Levels[LevelIndex].IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Level Index: [%d] is Invalid \n source [%s]"),LevelIndex, *GetNameSafe(this));
		return;
	}

	OpenLevelByName(FName(*Levels[LevelIndex].GetLongPackageName()));

}

void ULoadingScreen::OpenLevelByLevel(TSoftObjectPtr<UWorld> LevelToLoad)
{
	checkf(!LevelToLoad.IsNull(), TEXT("Cant find Level into: [%s] is NULL. \nSource: [%s]"), *LevelToLoad.GetAssetName(), *GetNameSafe(this))

	OpenLevelByName(FName(*LevelToLoad.GetLongPackageName()));
}

void ULoadingScreen::OpenLevelByName(FName LevelToLoad)
{
	SetVisibility(ESlateVisibility::Visible);

	CurrentLevelNameToLoad = LevelToLoad;

	StartLevelTimer();
}

void ULoadingScreen::StartLevelTimer()
{
	FTimerHandle Timer;

	float LoadingTime = WaitSecondsForLoad * (bIsAffectedByTimeDilation ? KINDA_SMALL_NUMBER : 1.0f);

	GetWorld()->GetTimerManager().SetTimer(
		Timer,
		this,
		&ThisClass::OpenLevelByName,
		LoadingTime,
		false
	);

}

void ULoadingScreen::OpenLevelByName()
{
	if(Transition_Animation)
	{
		BindToAnimationFinished(Transition_Animation, TranstionEvent);
		PlayAnimationForward(Transition_Animation);
	}
	else
	{
		OpenLevel();
	}
}

void ULoadingScreen::OpenLevel()
{
	UGameplayStatics::OpenLevel(this, CurrentLevelNameToLoad);
}
