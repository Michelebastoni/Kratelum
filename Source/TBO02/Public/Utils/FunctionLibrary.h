// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// BP Functions
	UFUNCTION(BlueprintCallable, Category="Utils")
	static TSubclassOf<UObject> WeightedExtraction(TMap<TSubclassOf<UObject>, float> EntriesWeight);

	UFUNCTION(BlueprintCallable, Category="Utils")
	static void LaserTrace(USceneComponent* Origin, UNiagaraComponent* NS_Laser, UNiagaraComponent* NS_LaserImpact,
						   UPARAM(ref) float& LaserLength, float LaserExtendSpeed, float MaxLaserLength, float Damage, bool bSkipPlayer, bool bSkipEnemies);

	// NON-BP Functions
	template <typename T>
	static TArray<T*> GetAllActorsOfClassCasted(UWorld* World)
	{
		TArray<T*> OutArray;

		if (!World)
			return OutArray;

		for (TActorIterator<T> It(World); It; ++It)
		{
			T* Actor = *It;
			if (Actor)
				OutArray.Add(Actor);
		}

		return OutArray;
	}
};
