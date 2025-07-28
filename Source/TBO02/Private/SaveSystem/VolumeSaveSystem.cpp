#include "SaveSystem/VolumeSaveSystem.h"
#include "AudioSystem/Subsystem/LoopStationSubsystem.h"
#include "Utils/SavePathNamesUtil.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(VolumeSavingLog);

void UVolumeSaveSystem::SetNewSettings(EAudioSetting NewMusic_Setting, EAudioSetting NewSFX_Setting)
{
	Music_Settings = NewMusic_Setting;

	SFX_Settings = NewSFX_Setting;

	UGameplayStatics::SaveGameToSlot(this, SavePathNamesUtil::VOLUME_DATA_FILE, 0);

#if WITH_EDITOR
	UE_LOG(VolumeSavingLog, Log, TEXT("SAVED DATA: \nMusic_Setting: {%s} \nSFX_Setting: {%s}"),
		*GetEnumName(Music_Settings),
		*GetEnumName(SFX_Settings)
		);
#endif // WITH_EDITOR

}

void UVolumeSaveSystem::GetSettings(EAudioSetting& MusicsSetting, EAudioSetting& SFXSetting)
{
	MusicsSetting = Music_Settings;
	SFXSetting = SFX_Settings;
}

#if WITH_EDITOR
FString UVolumeSaveSystem::GetEnumName(EAudioSetting setting)
{
	switch(setting)
	{
	case EAudioSetting::High:
		return FString("High");
	case EAudioSetting::Medium:
		return FString("Medium");
	case EAudioSetting::Off:
		return FString("Off");
	default:
		return FString("Not Found");
	};

}
#endif
