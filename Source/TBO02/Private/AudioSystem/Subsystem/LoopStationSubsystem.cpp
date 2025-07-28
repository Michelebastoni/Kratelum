#include "AudioSystem/Subsystem/LoopStationSubsystem.h"

#include "AudioSystem/Actors/LoopPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSystem/VolumeSaveSystem.h"
#include "Utils/SavePathNamesUtil.h"

TObjectPtr<ALoopPlayer> ULoopStationSubsystem::LoopPlayer = nullptr;

EAudioSetting ULoopStationSubsystem::MusicSettings = EAudioSetting::High;
EAudioSetting ULoopStationSubsystem::SfxSettings = EAudioSetting::High;

TMap<EAudioSetting, float> ULoopStationSubsystem::MusicSettingVolumeMap;
TMap<EAudioSetting, float> ULoopStationSubsystem::SfxSettingVolumeMap;

void ULoopStationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	LoopPlayer = nullptr;

	UE_LOG(LogTemp, Display, TEXT("Loop Station subsystem initialized!"));

	AutoInitVolumeMaps();
	
	const FString DataPath = SavePathNamesUtil::VOLUME_DATA_FILE;

	if (const bool bPathExist = UGameplayStatics::DoesSaveGameExist(DataPath, 0); !bPathExist)
	{
		UE_LOG(LogTemp, Warning, TEXT("File %s does not exist, resetting to default Audio! \nSource: [%s]"), *DataPath,
		       *GetName());
	}
	else
	{
		// Actually load game data
		if (USaveGame* SaveData = UGameplayStatics::LoadGameFromSlot(DataPath, 0))
		{
			if (UVolumeSaveSystem* VolumeSaveData = Cast<UVolumeSaveSystem>(SaveData))
			{
				VolumeSaveData->GetSettings(MusicSettings, SfxSettings);
			}
		}
	}

	Super::Initialize(Collection);
}

ULoopStationSubsystem* ULoopStationSubsystem::GetInstance(const UWorld* World)
{
	if (World)
	{
		return World->GetSubsystem<ULoopStationSubsystem>();
	}
	return nullptr;
}

void ULoopStationSubsystem::SetLoopPlayer(ALoopPlayer* Player)
{
	if (Player)
	{
		LoopPlayer = Player;
	}
}

void ULoopStationSubsystem::AddLoop(const UAudioSwitchTrackData* NewLoop, const float FadeInDuration)
{
	if (!LoopPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LoopPlayer is null! Aborted... Try calling SetLoopPlayer and then AddLoop"));
		return;
	}

	LoopPlayer->AddLoop(NewLoop, FadeInDuration);
}

void ULoopStationSubsystem::StopLoop(const UAudioSwitchTrackData* LoopToStop, const float FadeOutDuration)
{
	if (!LoopPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LoopPlayer is null! Aborted... Try calling SetLoopPlayer and then StopLoop"));
		return;
	}

	LoopPlayer->StopLoop(LoopToStop, FadeOutDuration);
}

void ULoopStationSubsystem::SetVolume(USoundMix* SoundMixModifier, USoundClass* SoundClass, const bool bIsMusic,
                                      const EAudioSetting AudioSetting)
{
	float Volume;
	if (bIsMusic)
	{
		MusicSettings = AudioSetting;
		Volume = MusicSettingVolumeMap[AudioSetting];
	}
	else
	{
		SfxSettings = AudioSetting;
		Volume = SfxSettingVolumeMap[AudioSetting];
	}

	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixModifier, SoundClass, Volume);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMixModifier);
}

void ULoopStationSubsystem::Deinitialize()
{
	if (UVolumeSaveSystem* VolumeSaveGame = Cast<UVolumeSaveSystem>(
		UGameplayStatics::CreateSaveGameObject(UVolumeSaveSystem::StaticClass())))
	{
		VolumeSaveGame->SetNewSettings(MusicSettings, SfxSettings);
	}

	LoopPlayer = nullptr;
	
	UE_LOG(LogTemp, Display, TEXT("Loop Station subsystem deinitialized!"));
	Super::Deinitialize();
}

void ULoopStationSubsystem::AutoInitVolumeMaps()
{
	MusicSettingVolumeMap.Add(EAudioSetting::Off, 0.0f);
	MusicSettingVolumeMap.Add(EAudioSetting::Medium, 0.5f);
	MusicSettingVolumeMap.Add(EAudioSetting::High, 1.0f);

	SfxSettingVolumeMap.Add(EAudioSetting::Off, 0.0f);
	SfxSettingVolumeMap.Add(EAudioSetting::Medium, 0.5f);
	SfxSettingVolumeMap.Add(EAudioSetting::High, 1.0f);

	MusicSettings = EAudioSetting::High;
	SfxSettings = EAudioSetting::High;
}
