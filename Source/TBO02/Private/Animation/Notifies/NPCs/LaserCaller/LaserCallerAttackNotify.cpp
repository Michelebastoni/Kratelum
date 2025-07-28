// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/NPCs/LaserCaller/LaserCallerAttackNotify.h"

#include "NPCs/Characters/LaserCallerNpc.h"

void ULaserCallerAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

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

	if (ALaserCallerNpc* NPC = Cast<ALaserCallerNpc>(Owner))
	{
		NPC->CallLaser();
	}
}
