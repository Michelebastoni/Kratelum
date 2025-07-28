#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "KamikazeAttackNotify.generated.h"

/**
 * Anim Notify used to trigger the Attack method of the Kamikaze NPC.
 */
UCLASS()
class TBO02_API UKamikazeAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
