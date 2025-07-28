#include "Traps/Trap.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATrap::ATrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


//Makes sure the function is implemented in BP 
void ATrap::ToggleTrap_Implementation()
{
	//ensureMsgf(false, TEXT("ERROR: ToggleTrap not implemented in Blueprint in %s!"), *GetClass()->GetPathName());
	//UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);

	FString Message =  TEXT("ToggleTrap not implemented in ") + GetClass()->GetPathName() + TEXT(" !");
	FText FormattedMessage = FText::FromString(Message);
	FMessageLog("PIE").Error()->AddToken(FTextToken::Create(FormattedMessage));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PerformAction() not implemented!"));
}

