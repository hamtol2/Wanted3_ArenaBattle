// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	// 노드 이름 설정.
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result
		= Super::ExecuteTask(OwnerComp, NodeMemory);

	// NPC가 캐릭터를 바라보는 방향.
	APawn* ControllingPawn
		= OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터 구하기.
	APawn* TargetPawn = Cast<APawn>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(
			BBKEY_TARGET
		)
	);

	if (!TargetPawn)
	{
		return EBTNodeResult::Failed;
	}

	// AIPawn 구하기.
	IABCharacterAIInterface* AIPawn
		= Cast<IABCharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// NPC가 바라볼 방향 구하기.
	FVector LookVector
		= TargetPawn->GetActorLocation()
		- ControllingPawn->GetActorLocation();

	// 회전 값 구하기.
	FRotator TargetRot
		= FRotationMatrix::MakeFromX(LookVector).Rotator();

	// DeltaTime.
	UWorld* World = ControllingPawn->GetWorld();



	// 회전 속도.
	float TurnSpeed = AIPawn->GetAIAITurnSpeed();

	// 부드럽게 회전 처리.
	FRotator FinalRotator = FMath::RInterpTo(
		ControllingPawn->GetActorRotation(),
		TargetRot,
		World->GetDeltaSeconds(),
		TurnSpeed
	);

	// 회전 설정.
	ControllingPawn->SetActorRotation(FinalRotator);

	return EBTNodeResult::Succeeded;
}
