// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Minion.generated.h"

class UBoxComponent;
class ABoss;

UCLASS()
class TBO02_API AMinion : public APawn
{
	GENERATED_BODY()

public:
	// Functions
	AMinion();

	UFUNCTION(BlueprintCallable)
	void HandleDamage(float Damage, AActor* Harmer);

	UFUNCTION()
	void HandleHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOnDestroyed(AActor* DestroyedActor);

	// Variables 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float StartingHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	UClass* BarrelClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int Number;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Config", meta = (ToolTip = "Applied when moving."))
	float CollisionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Config")
	float HPIncreasePerc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SFX_DamageShield;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SFX_Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABoss* Boss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsShieldUp = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AMinion*> Minions;


	// Components 
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MinionMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CartMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ShieldMesh;

	/** Used to damage entities while moving */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* HitBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLifeBar();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
