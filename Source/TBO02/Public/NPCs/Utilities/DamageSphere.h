#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DamageSphere.generated.h"


class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TBO02_API UDamageSphere : public USphereComponent
{
	GENERATED_BODY()

	float SphereRadius;
	float SphereDamage;

public:
	UDamageSphere();
	virtual void BeginPlay() override;

	void Init(float Radius, float Damage);

	void Toggle(const bool Value);

	UFUNCTION()
	void TryDamageActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
