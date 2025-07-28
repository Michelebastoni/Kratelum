#include "Widgets/Widget_Credits/CreditsWidget.h"
#include "Controllers/GenericPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCreditsWidget::NativeConstruct()
{
	Player_Controller = Cast<AGenericPlayerController>(GetOwningPlayer());

	checkf(Player_Controller, TEXT("Player_Controller missing on %s"), *GetName());

	checkf(!HUB.IsNull(), TEXT("HUB_Level is missing on %s"), *GetName());

	Credits_Event.BindDynamic(this, &ThisClass::OnCreditsFinished);
	BindToAnimationFinished(Credits_Animation, Credits_Event);

	if(!Player_Controller->GetSubSystem())
	{
		Player_Controller->OnFinishSetUp.AddDynamic(this, &ThisClass::SetUp);
	}
	else
	{
		SetUp(Player_Controller);
	}

	Player_Controller->OnSkipCinematic.AddDynamic(this, &ThisClass::SkipCinematic_Notify);

	Super::NativeConstruct();
}

void UCreditsWidget::NativeDestruct()
{
	Credits_Event.Unbind();
	UnbindFromAnimationFinished(Credits_Animation, Credits_Event);

	Player_Controller->OnSkipCinematic.RemoveDynamic(this, &ThisClass::SkipCinematic_Notify);

	Super::NativeDestruct();
}

void UCreditsWidget::PlayCreditsAnimation()
{
	PlayAnimationForward(Credits_Animation);
}

void UCreditsWidget::OnCreditsFinished()
{
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	if(LevelName == HUB.GetAssetName())
	{
		RemoveFromParent();
		Player_Controller->SwitchIntoHUBContext();
	}
	else
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), HUB);
	}
}

void UCreditsWidget::SetUp(const AGenericPlayerController* GenericPlayerController)
{
	Player_Controller->SwitchIntoCinematicContext();
}

void UCreditsWidget::SkipCinematic_Notify()
{
	OnCreditsFinished();
}
