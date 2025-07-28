#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LaserCallerAttackNotify.generated.h"

/**
 *  Anim Notify used to trigger the Attack method of the Laser Caller NPC.
 */
UCLASS()
class TBO02_API ULaserCallerAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;	
};
