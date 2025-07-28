#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileMultipool.generated.h"

class AProjectileBase;
class APool;

UCLASS()
class TBO02_API AProjectileMultipool : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling", meta=(AllowPrivateAccess="true"))
	TArray<APool*> ProjectilesPools;
	
	UPROPERTY()
	TMap<UClass*, APool*> PoolsMap;

public:
	AProjectileMultipool();

protected:
	virtual void BeginPlay() override;

public:
	AProjectileBase* GetProjectileByType(TSubclassOf<AProjectileBase> Type);

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Multipool utilities",
		meta = (ToolTip =
			"Click this to spawn a new empty pool."))
	void AddPool();
#endif
};
