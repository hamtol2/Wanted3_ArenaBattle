// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "AIController.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterAIInterface.h"
#include "Engine/OverlapResult.h"

UBTService_Detect::UBTService_Detect()
{
	// 에디터에서 보이는 이름 설정.
	NodeName = TEXT("Detect");

	// TickNode 함수 실행 간격 설정 (단위: 초).
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(
	UBehaviorTreeComponent& OwnerComp, 
	uint8* NodeMemory, 
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 폰에 접근.
	APawn* ControllingPawn 
		= OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return;
	}

	// AI 인터페이스로 형변환
	IABCharacterAIInterface* AIPawn
		= Cast<IABCharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return;
	}

	// 감지 범위(반지름) 얻어오기.
	float DetectRadius = AIPawn->GetAIDetectRange();

	// 감지 처리를 위해 필요한 값 구하기.
	UWorld* World = ControllingPawn->GetWorld();
	// NPC 캐릭터의 현재 위치를 시야 감지 중심 위치로 사용.
	FVector Center = ControllingPawn->GetActorLocation();

	// 충돌 결과 배열 선언.
	TArray<FOverlapResult> Results;

	// 충돌 판정할 때 자기자신을 제외 시키기 위해 사용.
	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(Detect),
		false,
		ControllingPawn
	);

	// 오버랩 함수를 활용해 시야 감지.
	bool Result = World->OverlapMultiByChannel(
		Results,
		Center,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(DetectRadius),
		Params
	);

	// 충돌 감지 됐을 때 처리.
	if (Result)
	{

	}

}
