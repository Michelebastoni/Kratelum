#include "Animation/Notifies/NPCs/Kamikaze/KamikazeAttackNotify.h"

#include "NPCs/Characters/KamikazeNpc.h"

void UKamikazeAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (AKamikazeNpc* NPC = Cast<AKamikazeNpc>(Owner))
	{
		NPC->Explode(NPC);
	}
}
