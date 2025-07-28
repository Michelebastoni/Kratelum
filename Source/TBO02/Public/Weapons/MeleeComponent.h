// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterLogic/GenericCharacter.h"
#include "Components/ActorComponent.h"
#include "MeleeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TBO02_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere)
	int meleeDamage = 10;
	UPROPERTY(EditAnywhere)
	float meleeRange = 100;

private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* meleeCharacterMontage;
	UPROPERTY()
	TObjectPtr<AGenericCharacter> meleeHolder;

	float DamageMultiplier = 1.0f;
	
public:
	UFUNCTION(BlueprintCallable)
	AActor* Melee(UWorld* world, FVector targetPosition);
	UFUNCTION(BlueprintCallable)
	void SetMeleeHolder(AGenericCharacter* holder) {	meleeHolder = holder;	}
	UFUNCTION(BlueprintCallable)
	AGenericCharacter* GetMeleeHolder() {	return meleeHolder;	}

	//Animations
	UAnimMontage* GetMeleeCharacterMontage() const {return meleeCharacterMontage;}

	UFUNCTION(BlueprintCallable)
	void ApplyDamageMultiplier(const float MultiplierValue);

	UFUNCTION(BlueprintCallable)
	void ResetDamageMultiplier();
};
