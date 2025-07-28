#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "IntegritySystem/Interfaces/IntegrityHolder.h"
#include "GameFramework/Character.h"
#include "GenericCharacter.generated.h"


class UProjectileDataAsset;
class USkeletalMeshComponent;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCharacterDeathSignature, AGenericCharacter*, character, class AWeapon*, currentWeapon, const AActor*, harmer, class UIntegrity*, integrity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterHitSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChangeWeaponSignature, AWeapon*, weapon, const float, weaponDuration, bool, bIsBaseWeapon);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedMultiplierUpdated, const float, NewValue);

UCLASS(Abstract)
class TBO02_API AGenericCharacter : public ACharacter, public IIntegrityHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCharacterDeathSignature OnCharacterDeath;
	
	UPROPERTY(BlueprintAssignable)
	FCharacterHitSignature OnCharacterHit;
	
	UPROPERTY(BlueprintAssignable)
	FChangeWeaponSignature OnChangeWeapon;

	UPROPERTY(BlueprintAssignable)
	FOnSpeedMultiplierUpdated OnSpeedMultiplierUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess))
	FName chestSocketName = "spine_03";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess))
	FName itemSocketName = "Rifle Socket";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (AllowPrivateAccess))
	FName headSocketName = "head";
	
	// --------------------- TO REWORK  ------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class UInteractionComponent> interactionComponent;

	UFUNCTION(BlueprintCallable)
	void SetCurrentMeleeHolder(AWeapon* meleeWeapon);

	UPROPERTY()
	TObjectPtr<class UMeleeComponent> currentMelee;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UMeleeComponent* GetMeleeComponent() const { return currentMelee; };
	// ----------------------------------------------------------------
protected:

	// Reference to integrity component
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess))
	TObjectPtr<class UIntegrity> integrityComponent;

	// This value indicate if weapon can be spawn at the begin play
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Weapon")
	bool startWithWeapon = true;

	// Default weapon to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Weapon")
	TSubclassOf<class AWeapon> weaponClass;

	// This variable indicate the socket where the weapon, if spawned, has to be attached
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Weapon")
	FName weaponSocketName = "Rifle Socket";

	// This variable indicate the default animation to trigger when character being hit
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Reactions")
	class UAnimMontage * reactionMontage = nullptr;

	// Indicate the vfx when being hit
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Reactions")
	class UNiagaraSystem* bloodImpact = nullptr;

	// This value indicate the variable animation snapshot name to save to interact into animation blueprint when character is dead
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Reactions")
	FName deathPoseNameSnapshot = "Death";

	// This value indicate the threshold where physics linear and angular velocity have to reach to disable physics simulation
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Reactions")
	float physicsThreshold = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Debug")
	bool showMessage = true;
	
	// Param that allow the character movement
	uint8 bCanMove : 1;

	// Param that allow tha character rotation
	uint8 bCanRotate : 1;

	// Param that allow "reload speed multiplier" var from weapon
	uint8 bIsAffectedByReloadMultiplier : 1;

	// Bool that indicate if character is Alive
	uint8 bIsAlive : 1;

	// Bool that indicate wheter character is going to freeze (stop physics simulation)
	uint8 bIsAwaitingFreeze : 1;

	// The max speed at which character can move
	float maxSpeed = 0.0f;
	
	float MovementSpeedMultiplier = 1.0f;
	
	// This value indicates the timer relative to stop the physics simulation (at the end of this timer the physics simulation will be stopped)
	FTimerHandle freezeTimer;

	FTimerHandle RagdollTimer;
	
	FCollisionQueryParams CharacterParam;

	// Weapon ref
	UPROPERTY()
	TObjectPtr<class AWeapon> currentWeapon;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> outlineMatInstance;

	UPROPERTY(Transient)
	USkeletalMeshComponent* MeshPtr;
	
	UPROPERTY(Transient)
	UCapsuleComponent* CapsulePtr;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	virtual void HandleCharacterDeath(const AActor* character, const AActor* harmer, const class UIntegrity* component, const float currentIntegrity);
	
	UFUNCTION()
	virtual void HandleOnChangeIntegrity(const UIntegrity* integrityComp, const float previousIntegrityValue, const float  currentIntegrityValue, const float changedValue, const float integrityValueRate, AActor* invader);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeWeaponParameters(class UWeaponParametersDataAsset* newWeaponAsset, float weaponDuration, bool bIsWeaponBase);
public:
	// Constructor
	AGenericCharacter();
	
	UFUNCTION(BlueprintCallable)
	void SpawnWeapon();

	// These are function that indicate character condition to do actions
	UFUNCTION(BlueprintCallable)
	virtual bool CanSee(FVector startPoint, FVector target);

	// These are the interface "Integrity Holder" functions
	UFUNCTION(BlueprintCallable)
	void Damage_Implementation(float damage, AActor* harmer) override;
	
	UFUNCTION(BlueprintCallable)
	void Restore_Implementation(float restoreValue, AActor* harmer) override;

	// These are the functions that handle the character behaviour
	void FreezeCharacter();
	void UnfreezeCharacter();
	virtual void ApplyImpact();
	
	UFUNCTION(BlueprintCallable)
	void SetRagdoll() const;
	
	UFUNCTION(BlueprintCallable)
	void UnsetRagdoll() const;

	//------------------ Getter -----------------------------------
	FCollisionQueryParams GetCharacterCollisionParamas() const { return CharacterParam; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UNiagaraSystem* GetBloodVfx() const { return bloodImpact; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class AWeapon* GetCurrentWeapon() const { return currentWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UIntegrity* GetIntegrityComponent() const { return integrityComponent; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UMaterialInstanceDynamic* GetOutlineMaterial() const { return outlineMatInstance; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float const GetMaxSpeed() const { return maxSpeed; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanCharacterRotate() const { return bCanRotate; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanCharacterMove() const { return bCanMove; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlive() const { return bIsAlive; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAwaitingFreeze() const { return bIsAwaitingFreeze; }
	
	UFUNCTION(BlueprintCallable)
	virtual float GetMovementSpeedMultiplier() const { return MovementSpeedMultiplier; }
	// --------------------------------------------------------
	

	// --------------------- Setter ----------------------------
	UFUNCTION(BlueprintCallable)
	void SetCurrentWeapon(class AWeapon* newWeapon, const float weaponDuration = 0, bool bIsWeaponBase = false);
	
	UFUNCTION(BlueprintCallable)
	void SetMaxSpeed(float newMaxSpeed);
	
	UFUNCTION(BlueprintCallable)
	void ToggleCharacterMovement(bool canMoving) { bCanMove = canMoving; };
	
	UFUNCTION(BlueprintCallable)
	void ToggleCharacterRotation(bool canRotating) { bCanRotate = canRotating; }
	
	UFUNCTION(BlueprintCallable)
	void ToggleCharacterAlive(bool alive) { bIsAlive = alive; };
	
	UFUNCTION(BlueprintCallable)
	void ToggleAwaitingFreeze(bool isAwaitFreeze) { bIsAwaitingFreeze = isAwaitFreeze; };
	// ---------------------------------------------------------
	
	UFUNCTION(BlueprintCallable)
	bool MakeAOEDamage(float radius, float damage,
	                                 const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, bool drawDebug,
	                                 USoundBase*
	                                 explositonSound, UNiagaraSystem* damageVFX);
	
	FCollisionObjectQueryParams SetCollisionObjectParams(const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes);
	
	UFUNCTION(BlueprintCallable)
	void ChangeWeaponProjectile(class UProjectileDataAsset* newProjectile);
	
	UFUNCTION(BlueprintCallable)
	virtual void ApplyMovementSpeedMultiplier(const float MultiplierValue)
	{
		MovementSpeedMultiplier = MultiplierValue;
		OnSpeedMultiplierUpdated.Broadcast(MovementSpeedMultiplier);
	};
	
	UFUNCTION(BlueprintCallable)
	virtual void ResetMovementSpeedMultiplier()
	{
		MovementSpeedMultiplier = 1.0f;
		OnSpeedMultiplierUpdated.Broadcast(1.0f);
	};
};
