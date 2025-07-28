#include "KratelumGameInstance.h"

#include "GameFramework/GameUserSettings.h"

void UKratelumGameInstance::Init()
{
	Super::Init();

	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		Settings->SetFrameRateLimit(120.0f);
		Settings->ApplySettings(false);
	}
}
