#pragma once

#include "CoreMinimal.h"
#include "Minion.h"
#include "Projectile_Boss.h"
#include "GameFramework/Pawn.h"
#include "IntegritySystem/Integrity.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "EngineUtils.h"
#include "Templates/SharedPointer.h"
#include "Boss.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;


USTRUCT(BlueprintType)
struct FProjectileParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float LifeSpan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	bool bFollowPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float HomingStrength;

	FProjectileParameters()
	{
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		Scale = 1.0f;
		Speed = 1.0f;
		LifeSpan = 1.0f;
		bFollowPlayer = false;
		HomingStrength = 0;
	}

	FProjectileParameters(const FVector& Location, const FRotator& Rotation, float Scale, float Speed, float LifeSpan,
	                      bool bFollowPlayer, float HomingStrength)
		: Location(Location),
		  Rotation(Rotation),
		  Scale(Scale),
		  Speed(Speed),
		  LifeSpan(LifeSpan),
		  bFollowPlayer(bFollowPlayer),
		  HomingStrength(HomingStrength)
	{
	}
};

USTRUCT(BlueprintType)
struct FLaserData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaserSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving = false;
	FTimerHandle TimerHandle;

	FLaserData()
	{
		NiagaraComponent = nullptr;
		Location1 = FVector::ZeroVector;
		Location2 = FVector::ZeroVector;
		LaserSpeed = 0.0f;
		bIsMoving = false;
	}

	FLaserData(UNiagaraComponent* NiagaraComponent, const FVector& Location1, const FVector& Location2,
		float LaserSpeed, bool bIsMoving, const FTimerHandle& TimerHandle)
		: NiagaraComponent(NiagaraComponent),
		  Location1(Location1),
		  Location2(Location2),
		  LaserSpeed(LaserSpeed),
		  bIsMoving(bIsMoving),
		  TimerHandle(TimerHandle)
	{
	}
};

UCLASS()
class TBO02_API ABoss : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoss();

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	float StartingHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AProjectile_Boss> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category="Config")
	UNiagaraSystem* LaserNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category="Config")
	float MinionLaserDamage;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Config")
	float MinionLaserAnticipationDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Transient")
	TArray<AMinion*> Minions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Transient")
	TMap<UNiagaraComponent*, FLaserData> LasersDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	bool bIsLaserActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	bool bIsShieldUp = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	bool bIsMinionsLaserActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SFX_DamageShield;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SFX_Damage;

	// Functions
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(FProjectileParameters ProjectileParams);

	UFUNCTION(BlueprintCallable)
	void SpawnBurstTowardPlayer(float Duration, float SpawnDelay, float AngleVariance, float SpeedVarianceMulti,
	                            const FProjectileParameters&
	                            ProjectileParams);

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(int SpawnPointCount, float SpawnRadius, float AngleOffsetRad,
	                      FProjectileParameters ProjectileParameters);

	UFUNCTION(BlueprintCallable)
	void MinionsLaserAttack(float LaserSpeed);

	// If bActive true the shield will be up, false deactivates it. 
	UFUNCTION(BlueprintCallable)
	void SetShieldActive(bool bActive);

	UFUNCTION(BlueprintCallable)
	void HandleDamage(float Damage, AActor* Harmer);

	UFUNCTION()
	void ToggleMinionLaserVisibility(UNiagaraComponent* NiagaraComponent, float ElapsedTime);

protected:
	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShieldMesh;

	// Functions
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnProjectileTowardPlayer(FProjectileParameters& ProjectileParams, float AngleVariance,
	                                 float SpeedVarianceMult);

	UFUNCTION()
	void StopTimers(FTimerHandle TimerHandle, FTimerHandle StopHandle);

	UFUNCTION()
	void MoveLasers();

	template <typename T>
	TArray<T*> GetAllActorsOfClass(UWorld* World);
};

template <typename T>
TArray<T*> ABoss::GetAllActorsOfClass(UWorld* World)
{
	TArray<T*> FoundActors;

	if (!World)
	{
		// Return if World is unvalid
		return FoundActors;
	}

	for (TActorIterator<T> It(World); It; ++It)
	{
		FoundActors.Add(*It);
	}

	return FoundActors;
}
