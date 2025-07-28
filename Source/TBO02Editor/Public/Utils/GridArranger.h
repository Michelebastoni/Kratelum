#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GridArranger.generated.h"

UCLASS()
class TBO02EDITOR_API AGridArranger : public AActor
{
	GENERATED_BODY()
	
public:
	AGridArranger()
	{
		PrimaryActorTick.bCanEverTick = false;
	}

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Grid Arrange")
	void ArrangeActorsInGrid() const
	{
		{
			TArray<AActor*> Actors;

			for (TActorIterator<AActor> It(GetWorld()); It; ++It)
			{
				AActor* Actor = *It;
				if (Actor && !ActorsToExclude.Contains(Actor) && Actor != this)
				{
					Actors.Add(Actor);
				}
			}

			const int32 TotalActors = Actors.Num();
			if (TotalActors == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("No actor to move!"));
				return;
			}

			const int32 GridSize = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(TotalActors)));

			for (int32 Index = 0; Index < TotalActors; ++Index)
			{
				int32 Row = Index / GridSize;
				int32 Col = Index % GridSize;

				FVector NewLocation = FVector(Col * Spacing, Row * Spacing, 0);
				Actors[Index]->SetActorLocation(NewLocation);
			}

			UE_LOG(LogTemp, Log, TEXT("Actors replaced in grid: %dx%d"), GridSize, GridSize);
		}
	};
#endif

	UPROPERTY(EditAnywhere, Category = "Grid Arrange")
	TArray<AActor*> ActorsToExclude;

	UPROPERTY(EditAnywhere, Category = "Grid Arrange")
	float Spacing = 200.f;

};
