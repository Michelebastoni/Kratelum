#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AudioSystem/Subsystem/LoopStationSubsystem.h"
#include "VolumeSaveSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(VolumeSavingLog, Log, All);

/**
 * 
 */
UCLASS()
class TBO02_API UVolumeSaveSystem : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	EAudioSetting Music_Settings;

	UPROPERTY()
	EAudioSetting SFX_Settings;

public:
	UFUNCTION(BlueprintCallable)
	void SetNewSettings(EAudioSetting NewMusic_Setting, EAudioSetting NewSFX_Setting);

	UFUNCTION(BlueprintCallable)
	void GetSettings(EAudioSetting& MusicsSetting, EAudioSetting& SFXSetting);

#if WITH_EDITOR
	FString GetEnumName(EAudioSetting setting);
#endif // WITH_EDITOR


};
