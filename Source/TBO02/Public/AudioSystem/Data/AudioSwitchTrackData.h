#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AudioSwitchTrackData.generated.h"

UCLASS(BlueprintType)
class TBO02_API UAudioSwitchTrackData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> LoopToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USoundBase>> LoopsToStop;
};
