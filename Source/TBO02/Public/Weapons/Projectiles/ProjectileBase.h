#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolingSystem/Interfaces/Poolable.h"
#include "Weapons/ProjectileDataAssets/ProjectileDataAsset.h"
#include "ProjectileBase.generated.h"

class UProjectileDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, AActor*, hitActor, FHitResult, hitResult, AWeapon*,
                                               weapon);

class AWeapon;
class UProjectileMovementComponent;

UCLASS()
class TBO02_API AProjectileBase : public AActor, public IPoolable
{
	GENERATED_BODY()

	//Variables
public:
	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

protected:
	UPROPERTY()
	AWeapon* weaponInstigator;
	UPROPERTY()
	UProjectileDataAsset* projectileDataAsset;

	// Pool utils
	UPROPERTY(Transient)
	APool* ProjectilePool;
	float Lifetime;
	FTimerHandle PoolingTimerHandle;
	UPROPERTY(Transient)
	bool bIsPooled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UProjectileMovementComponent> projectileMovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* projectileMesh;

	//Functions
public:
	AProjectileBase();
	UStaticMeshComponent* GetProjectileMesh() const { return projectileMesh; }
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeaponInstigator() const { return weaponInstigator; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponInstigator(AWeapon* newWeaponInstigator) { weaponInstigator = newWeaponInstigator; }

	UFUNCTION(BlueprintCallable)
	UProjectileMovementComponent* GetProjectileMovementComponent() const { return projectileMovementComponent; }

	UFUNCTION(BlueprintCallable)
	void SetProjectileDataAsset(UProjectileDataAsset* newProjectileDataAsset)
	{
		projectileDataAsset = newProjectileDataAsset;
		Lifetime = (projectileDataAsset->projectileRange * 100) / projectileDataAsset->projectileSpeed;
		GetWorldTimerManager().ClearTimer(PoolingTimerHandle);
		GetWorldTimerManager().SetTimer(PoolingTimerHandle, this, &AProjectileBase::OnLifetimeEnded, Lifetime);
	}

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void OnLifetimeEnded();

private:
	UFUNCTION()
	void ProjectileOnTriggerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                              const FHitResult& SweepResult);

	// Pooling system
public:
	virtual bool GetIsInUse_Implementation() override;
	virtual void Initialize_Implementation(APool* Pool) override;
	virtual void OnObjectPooled_Implementation() override;
	virtual void OnObjectReturnToPool_Implementation() override;
};
