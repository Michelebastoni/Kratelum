// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAdvanced.generated.h"

/**
 * 
 */
UCLASS()
class TBO02_API UBTTask_MoveToAdvanced : public UBTTask_MoveTo
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
