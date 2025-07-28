#include "Widgets/Widget_Overflow/OverflowProgressBar.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "OverflowSystem/Overflow.h"

void UOverflowProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	checkf(Player, TEXT("Failed to find Player into %s"), *GetName());

	Player->GetOverflowComponent()->OnChangeOverflow.AddDynamic(this, &ThisClass::UpdateOverflowPercent);

	ForceSetProgressBar(Player->GetOverflowComponent()->GetCurrentOverflowRate());

}

void UOverflowProgressBar::NativeDestruct()
{
	Super::NativeDestruct();

	Player->GetOverflowComponent()->OnChangeOverflow.RemoveDynamic(this, &ThisClass::UpdateOverflowPercent);
}

void UOverflowProgressBar::UpdateOverflowPercent(UOverflow* overflow, const float nextOverflowValue, const float overflowRate)
{
	changeProgressBarDuration = overflow->GetSmoothChangeValueTime();
	SetProgressBarValue(overflowRate);
}
