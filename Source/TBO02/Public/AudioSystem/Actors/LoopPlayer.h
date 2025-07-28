#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoopPlayer.generated.h"

enum class EAudioSetting : uint8;
class UAudioSwitchTrackData;
class UQuartzClockHandle;

UCLASS()
class TBO02_API ALoopPlayer : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	UQuartzClockHandle* Metronome;

	UPROPERTY(Transient)
	FOnQuartzCommandEventBP QuartzCommandDelegate;

	UPROPERTY(Transient)
	FQuartzQuantizationBoundary QuantizationBoundary;

	UPROPERTY(Transient)
	TArray<UAudioComponent*> LoopComponents;

public:	
	UPROPERTY(EditDefaultsOnly)
	USoundClass* MusicSoundClass;

	UPROPERTY(EditDefaultsOnly)
	USoundMix* MusicSoundClassMix;
	
	UPROPERTY(EditDefaultsOnly)
	USoundClass* SfxSoundClass;

	UPROPERTY(EditDefaultsOnly)
	USoundMix* SfxSoundClassMix;
	
protected:	
	UPROPERTY(EditInstanceOnly)
	TArray<TObjectPtr<UAudioSwitchTrackData>> StartingLoops;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void AddLoop(const UAudioSwitchTrackData* NewLoop, const float FadeInDuration = 0.0f);

	UFUNCTION(BlueprintCallable)
	void StopLoop(const UAudioSwitchTrackData* NewLoop, const float FadeOutDuration = 0.0f);

	UFUNCTION(BlueprintCallable)
	void StopMatchingLoops(const TArray<USoundBase*>& LoopsToStop);

	UFUNCTION(BlueprintCallable)
	void StopAll();

private:
	UAudioComponent* FindAvailableAudioComponent();
};
