#include "Widgets/SkipText.h"
#include "Controllers/GenericPlayerController.h"

void USkipText::NativeConstruct()
{
	Player_Controller = Cast<AGenericPlayerController>(GetOwningPlayer());

	checkf(Player_Controller, TEXT("Player_Controller missing on %s"), *GetName());

	Player_Controller->OnInputReceived.AddDynamic(this, &ThisClass::OnInputRecived_Notify);

	bCanAnimationBegin = true;

	if(SkipText_Animation)
	{
		SkipAnimation_Event.BindDynamic(this, &ThisClass::MakeSkipAnimationPlayable);
		BindToAnimationFinished(SkipText_Animation, SkipAnimation_Event);
		PlaySkipAnimation(false, 2.0f);
	}

	Super::NativeConstruct();
}

void USkipText::NativeDestruct()
{
	Player_Controller->OnInputReceived.RemoveDynamic(this, &ThisClass::OnInputRecived_Notify);

	if(SkipText_Animation)
	{
		SkipAnimation_Event.Unbind();
		UnbindFromAnimationFinished(SkipText_Animation, SkipAnimation_Event);
	}

	Super::NativeDestruct();
}

void USkipText::PlaySkipAnimation(bool bForward, float timeToStart)
{
	if(bCanAnimationBegin)
	{
		PlayAnimation(
			SkipText_Animation,
			timeToStart,
			1,
			bForward ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse
		);

		bCanAnimationBegin = false;
	}
}

void USkipText::ChangeText_Implementation(bool isGamePadInput)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			76,
			100.0f,
			FColor::Orange,
			FString::Printf(TEXT("Change_Text function has not a implementation on %s"), *GetName())
		);
	}
}

void USkipText::OnInputRecived_Notify(const FKey& Key, bool isGamepadInput)
{
	ChangeText(isGamepadInput);
	PlaySkipAnimation();
}
