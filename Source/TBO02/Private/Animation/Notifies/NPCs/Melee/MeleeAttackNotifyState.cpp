#include "Animation/Notifies/NPCs/Melee/MeleeAttackNotifyState.h"

#include "NPCs/Characters/MeleeNpc.h"

void UMeleeAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (const AMeleeNpc* NPC = Cast<AMeleeNpc>(Owner))
	{
		NPC->ToggleDamageSphere(true);
	}
}

void UMeleeAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (const AMeleeNpc* NPC = Cast<AMeleeNpc>(Owner))
	{
		NPC->ToggleDamageSphere(false);
	}
}
