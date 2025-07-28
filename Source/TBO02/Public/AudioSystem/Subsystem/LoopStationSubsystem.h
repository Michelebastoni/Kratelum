#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LoopStationSubsystem.generated.h"

class UAudioSwitchTrackData;
class ALoopPlayer;

UENUM(BlueprintType)
enum class EAudioSetting : uint8
{
	Off,
	Medium,
	High
};

UCLASS()
class TBO02_API ULoopStationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	static TObjectPtr<ALoopPlayer> LoopPlayer;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	static EAudioSetting MusicSettings;
	static EAudioSetting SfxSettings;

	static TMap<EAudioSetting, float> MusicSettingVolumeMap;
	static TMap<EAudioSetting, float> SfxSettingVolumeMap;

	static ULoopStationSubsystem* GetInstance(const UWorld* World);

	UFUNCTION(BlueprintCallable)
	static void SetLoopPlayer(ALoopPlayer* Player);

	UFUNCTION(BlueprintCallable)
	static EAudioSetting GetMusicSettings() { return MusicSettings; };

	UFUNCTION(BlueprintCallable)
	static EAudioSetting GetSfxSettings() { return SfxSettings; };

	UFUNCTION(BlueprintCallable)
	static float GetMusicVolume() { return MusicSettingVolumeMap[MusicSettings]; };

	UFUNCTION(BlueprintCallable)
	static float GetSfxVolume() { return SfxSettingVolumeMap[SfxSettings]; };
	
	UFUNCTION(BlueprintCallable)
	static void AddLoop(const UAudioSwitchTrackData* NewLoop, const float FadeInDuration = 0.0f);

	UFUNCTION(BlueprintCallable)
	static void StopLoop(const UAudioSwitchTrackData* LoopToStop, const float FadeOutDuration = 0.0f);

	UFUNCTION(BlueprintCallable)
	void SetVolume(USoundMix* SoundMixModifier, USoundClass* SoundClass, const bool bIsMusic,
	               const EAudioSetting AudioSetting);

private:
	static void AutoInitVolumeMaps();
};
