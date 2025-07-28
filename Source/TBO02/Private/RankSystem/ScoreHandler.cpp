#include "RankSystem/ScoreHandler.h"
#include "Kismet/GameplayStatics.h"
#include "NPCs/Characters/NpcBase.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "NPCs/Subsystems/NPCSubsystem.h"
#include "Widgets/Widget_Score/ScoreWidget.h"

#include "Traps/Trap.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif

void AScoreHandler::IncreaseScore(int scoreToAdd)
{
	CurrentScore += scoreToAdd;
	UE_LOG(LogTemp, Display, TEXT("Current score: %d"), CurrentScore);
}

void AScoreHandler::IncreaseScoreMultiplier()
{
	CurrentMultiplier += multiplierDelta;
	CurrentMultiplier = FMath::Clamp(CurrentMultiplier, initialScoreMultiplier, multiplierCap);
	UE_LOG(LogTemp, Display, TEXT("Current multiplier: %.2f"), CurrentMultiplier);
}

AScoreHandler::AScoreHandler() : bIsComboStarted(false), bIsTrapKill(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AScoreHandler::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	CurrentScore = initialScore;
	CurrentMultiplier = initialScoreMultiplier;
	CurrentTrapMultiplier = 1.0f;

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.AddDynamic(this, &ThisClass::HandleScore);
	}

	if (scoreWidgetClass && Player)
	{
		ScoreWidget = Cast<UScoreWidget>(CreateWidget(GetWorld()->GetGameInstance(), scoreWidgetClass));
		ScoreWidget->AddToViewport();
		ScoreWidget->SetUp(this);
		Player->OnCharacterHit.AddDynamic(this, &ThisClass::ResetScoreMultiplier);
	}
}

void AScoreHandler::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ScoreWidget)
	{
		ScoreWidget->UpdateScoreBar(GetComboTimeRate());
	}

#if WITH_EDITOR
	if (bEnableDebugMessage)
		GetInfo();
#endif // WITH_EDITOR
}

void AScoreHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UNPCSubsystem* NPCSubsystem = UNPCSubsystem::GetInstance(GetWorld()))
	{
		NPCSubsystem->OnEnemyDied.RemoveDynamic(this, &ThisClass::HandleScore);
	}
}

FString AScoreHandler::GetCurrentRankAsString() const
{
	switch (CurrentRank)
	{
	case ERank::K :
		return FString("K");
	case ERank::S:
		return FString("S");
	case ERank::A:
		return FString("A");
	case ERank::B:
		return FString("B");
	case ERank::C:
		return FString("C");
	case ERank::D:
		return FString("D");
	case ERank::None:
		return FString("-");
	default:
		return FString("-");
	}
}

ERank AScoreHandler::GetRankFromScore(const int score) const
{
	ERank rank = ERank::None;

	float value = 0.0f;
	for (const TPair<ERank, int> pair : rankSettings)
	{
		if (score >= pair.Value && value < pair.Value)
		{
			value = pair.Value;
			rank = pair.Key;
		}
	}

	return rank;
}

void AScoreHandler::HandleComboTimer(bool restart)
{
	if (restart)
	{
		GetWorldTimerManager().ClearTimer(ComboTimer);
	}

	GetWorldTimerManager().SetTimer(
		ComboTimer,
		this,
		&ThisClass::ResetScoreMultiplier,
		comboTime,
		false
	);
}

void AScoreHandler::ResetScoreMultiplier()
{
	CurrentMultiplier = initialScoreMultiplier;

	CurrentKillPoint = 0;

	CurrentTrapMultiplier = 1.0f;

	GetWorldTimerManager().ClearTimer(ComboTimer);

	bIsComboStarted = false;

	bIsTrapKill = false;

	OnScoreComboEnd.Broadcast(this, false);
}

void AScoreHandler::HandleScore(const AActor* killedActor, const int pointValue, const AActor* harmer)
{
	// handle combo timer
	if (ComboTimer.IsValid())
	{
		HandleComboTimer(true); // -> reset and set combo timer
	}
	else
	{
		HandleComboTimer(false); // -> set first combo timer
	}

	// Handle combo multiplier
	if (bIsComboStarted)
	{
		IncreaseScoreMultiplier();
	}
	else
	{
		bIsComboStarted = true;
		OnScoreComboStarted.Broadcast(this);
	}

	// Handle trap multiplier
	if (harmer)
	{

		if (const ATrap* Trap = Cast<ATrap>(harmer))
		{
			// Override logic because Game Designers
			// don't know what to do with their lives
			if (Trap->bOverrideTrapMultiplier)
			{
				CurrentTrapMultiplier = Trap->TrapMultiplier;
			}
			else
			{
				CurrentTrapMultiplier = trapMultiplier;
			}

			bIsTrapKill = true;
		}
		else
		{
			CurrentTrapMultiplier = 1.0f;
			bIsTrapKill = false;
		}
	}

	CurrentKillPoint = pointValue * CurrentMultiplier * CurrentTrapMultiplier;
	PreviousScore = CurrentScore;
	IncreaseScore(CurrentKillPoint);

	UpdateRank();

	OnScoreUpdate.Broadcast(this, bIsTrapKill);
}

float AScoreHandler::GetComboTimeRate() const
{
	if (comboTime == 0.0f)
	{
		return 0.0f;
	}

	if (!ComboTimer.IsValid())
	{
		return 0.0f;
	}

	float currentComboValue = GetWorldTimerManager().GetTimerRemaining(ComboTimer);

	if (currentComboValue < KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	return currentComboValue / comboTime;
}

#if WITH_EDITOR

void AScoreHandler::GetInfo()
{
	FFormatNamedArguments args;
	args.Add("Final Score", CurrentScore);
	args.Add("kill point", CurrentKillPoint);
	args.Add("Multiplier", CurrentMultiplier);
	args.Add("TimeRate", GetComboTimeRate());
	args.Add("Trap Mulitplier", CurrentTrapMultiplier);


	const FText& debugText = FText::Format(NSLOCTEXT(
		"MyNamespace",
		"MyKey",
		" Final score: {Final Score} \n "
		" Current Multiplier: {Multiplier} \n"
		" Current kill points: {kill point} \n"
		" Current trap multiplier: {Trap Mulitplier} \n"
		" Current combo time: {TimeRate}"), args);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			67,
			1.0f,
			FColor::Cyan,
			debugText.ToString()
		);
	}
}

void AScoreHandler::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	CheckForAnotherScoreHandler();
}

void AScoreHandler::CheckForAnotherScoreHandler()
{
	if (!GetWorld() || GetWorld()->WorldType != EWorldType::Editor)
		return;

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AScoreHandler::StaticClass(), FoundActors);

	if (FoundActors.Num() > 1)
	{
		ShowEditorNotification(TEXT("More than one Score Handler exist!!!"));
	}
}

void AScoreHandler::ShowEditorNotification(const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.Image = FCoreStyle::Get().GetBrush(TEXT("MessageLog.Error"));
	Info.ExpireDuration = 5.0f;
	Info.bFireAndForget = true;
	Info.bUseSuccessFailIcons = true;
	Info.Hyperlink = FSimpleDelegate::CreateLambda([this]()
	{
		const FString DocsURL = TEXT("https://docs.unrealengine.com/");
		FPlatformProcess::LaunchURL(*DocsURL, nullptr, nullptr);
	});

	Info.HyperlinkText = FText::FromString(TEXT("-- > Click here to Fix it <--"));
	//("GoToDocs", "Go to Documentation...");

	FSlateNotificationManager::Get().AddNotification(Info);
}
#endif // WITH_EDITOR
