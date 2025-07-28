#include "IntegritySystem/UI/IntegirtyWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterLogic/PlayerCharacter.h"
#include "IntegritySystem/Integrity.h"

void UIntegrityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	checkf(Player, TEXT("Player not found at Integrity Widget"));

	Integrity = Player->GetIntegrityComponent();

	check(ProgressBar);

	checkf(Integrity, TEXT("Integrity not found at Integrity Widget"));

	defaultHpColor = ProgressBar->GetFillColorAndOpacity();

	float currentIntegrityValue = Integrity->GetInitialIntegrityValue();
	ForceSetProgressBar(currentIntegrityValue / Integrity->GetMaxIntegrityValue());
	
	Integrity->OnChangeIntegrity.AddDynamic(this, &UIntegrityWidget::OnHealthChanged);

}

void UIntegrityWidget::OnHealthChanged(const UIntegrity* integrityComp, const float previousIntegrityValue, const float  currentIntegrityValue, const float changedValue, const float integrityValueRate, AActor* invader)
{
	SetProgressBarValue(integrityValueRate);
}

void UIntegrityWidget::CheckProgressBarStatus()
{
	Super::CheckProgressBarStatus();

	if (currentProgressBarPercent < lowHpRatio && ProgressBar)
	{
		ProgressBar->SetFillColorAndOpacity(lowHpColor);
	}
	else
	{
		ProgressBar->SetFillColorAndOpacity(defaultHpColor);
	}
	
	// glow here (?)
}

