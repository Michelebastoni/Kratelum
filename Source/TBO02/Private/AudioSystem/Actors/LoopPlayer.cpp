#include "AudioSystem/Actors/LoopPlayer.h"

#include "AudioSystem/Data/AudioSwitchTrackData.h"
#include "AudioSystem/Subsystem/LoopStationSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Quartz/QuartzSubsystem.h"
#include "Sound/SoundClass.h"

void ALoopPlayer::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	if (!ULoopStationSubsystem::GetInstance(World))
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Loop Station subsystem! Aborting initialization!"))
	}

	ULoopStationSubsystem::SetLoopPlayer(this);

	// ULoopStationSubsystem::GetInstance(World)->SetVolume(MusicSoundClassMix, MusicSoundClass, true,
	//                                                      ULoopStationSubsystem::MusicSettings);
	// ULoopStationSubsystem::GetInstance(World)->SetVolume(SfxSoundClassMix, SfxSoundClass, false,
	//                                                      ULoopStationSubsystem::SFXSettings);

	FQuartzClockSettings QuartzSettings;

	QuartzSettings.TimeSignature.NumBeats = 4;
	QuartzSettings.TimeSignature.BeatType = EQuartzTimeSignatureQuantization::QuarterNote;

	QuartzSettings.bIgnoreLevelChange = false;

	QuantizationBoundary.Quantization = EQuartzCommandQuantization::Beat;
	QuantizationBoundary.Multiplier = 1.0f;
	QuantizationBoundary.CountingReferencePoint = EQuarztQuantizationReference::TransportRelative;
	QuantizationBoundary.bFireOnClockStart = true;
	QuantizationBoundary.bCancelCommandIfClockIsNotRunning = false;
	QuantizationBoundary.bResetClockOnQueued = false;
	QuantizationBoundary.bResumeClockOnQueued = false;

	Metronome = UQuartzSubsystem::Get(World)->CreateNewClock(GetWorld(), FName("110BPM"), QuartzSettings, false,
	                                                         true);

	Metronome->SetBeatsPerMinute(World, QuantizationBoundary, QuartzCommandDelegate, Metronome, 110.0f);

	Metronome->StartClock(World, Metronome);

	ULoopStationSubsystem::GetInstance(World)->SetVolume(MusicSoundClassMix, MusicSoundClass, true, ULoopStationSubsystem::MusicSettings);
	ULoopStationSubsystem::GetInstance(World)->SetVolume(SfxSoundClassMix, SfxSoundClass, false, ULoopStationSubsystem::SfxSettings);
	
	for (int32 i = 0; i < StartingLoops.Num(); i++)
	{
		AddLoop(StartingLoops[i].Get());
	}
}

void ALoopPlayer::AddLoop(const UAudioSwitchTrackData* NewLoop, const float FadeInDuration)
{
	if (!NewLoop)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	StopMatchingLoops(NewLoop->LoopsToStop);

	for (int32 i = 0; i < LoopComponents.Num(); i++)
	{
		if (LoopComponents[i]->IsPlaying() && LoopComponents[i]->GetSound() == NewLoop->LoopToPlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Track already playing, recycling the one already existing."));
			return;
		}
	}

	UAudioComponent* AudioComponent = FindAvailableAudioComponent();

	if (AudioComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available audio components, creating one."));

		AudioComponent = UGameplayStatics::CreateSound2D(
			World, NewLoop->LoopToPlay.Get(), 1.0f, 1.0f, 0.0f,
			nullptr, false, false);

		LoopComponents.Add(AudioComponent);
		AudioComponent->PlayQuantized(World, Metronome, QuantizationBoundary, QuartzCommandDelegate);
		return;
	}

	AudioComponent->SetSound(NewLoop->LoopToPlay);

	AudioComponent->PlayQuantized(World, Metronome, QuantizationBoundary, QuartzCommandDelegate, 0, FadeInDuration);
}

void ALoopPlayer::StopLoop(const UAudioSwitchTrackData* NewLoop, const float FadeOutDuration)
{
	const int32 LoopComponentsCount = LoopComponents.Num();

	for (int32 i = 0; i < LoopComponentsCount; i++)
	{
		if (LoopComponents[i]->IsPlaying() && LoopComponents[i]->GetSound() == NewLoop->LoopToPlay)
		{
			if (FadeOutDuration > 0.0f)
			{
				LoopComponents[i]->FadeOut(FadeOutDuration, 0.0f, EAudioFaderCurve::Linear);
				return;
			}

			LoopComponents[i]->Stop();
		}
	}
}

void ALoopPlayer::StopMatchingLoops(const TArray<USoundBase*>& LoopsToStop)
{
	if (LoopsToStop.IsEmpty())
	{
		return;
	}

	const int32 LoopsToStopCount = LoopsToStop.Num();
	const int32 LoopComponentsCount = LoopComponents.Num();

	for (int32 i = 0; i < LoopsToStopCount; i++)
	{
		if (!LoopsToStop[i])
		{
			continue;
		}

		for (int32 j = 0; j < LoopComponentsCount; j++)
		{
			if (LoopComponents[j]->IsPlaying() && LoopComponents[j]->GetSound() == LoopsToStop[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("Stopping loop!."));
				LoopComponents[j]->Stop();
			}
		}
	}
}

void ALoopPlayer::StopAll()
{
	const int32 LoopComponentsCount = LoopComponents.Num();
	for (int32 i = 0; i < LoopComponentsCount; i++)
	{
		if (LoopComponents[i]->IsPlaying())
		{
			LoopComponents[i]->Stop();
		}
	}
}

UAudioComponent* ALoopPlayer::FindAvailableAudioComponent()
{
	const int32 LoopComponentsCount = LoopComponents.Num();
	for (int32 i = 0; i < LoopComponentsCount; i++)
	{
		if (!LoopComponents[i]->IsPlaying())
		{
			return LoopComponents[i];
		}
	}
	return nullptr;
}
