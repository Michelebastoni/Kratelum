#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectilePoolingSubsystem.generated.h"

class AProjectileBase;
class AProjectileMultipool;

UCLASS()
class TBO02_API UProjectilePoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	AProjectileMultipool* ProjectilePool;

public:
	static UProjectilePoolingSubsystem* GetInstance(const UWorld* World);
	AProjectileBase* GetProjectileByType(TSubclassOf<AProjectileBase> Type) const;

	void SetPool(AProjectileMultipool* Value);
	
};
