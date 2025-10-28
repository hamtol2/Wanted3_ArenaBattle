// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
 	// Stage Section.
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	// 스테이지 메시 애셋 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE"));
	if (StageMeshRef.Succeeded())
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	// 박스 콜리전 컴포넌트 생성.
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetupAttachment(Stage);
	// 박스 콜리전 크기 설정.
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	
	// 상대 위치 설정.
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	
	// 콜리전 설정.
	StageTrigger->SetCollisionProfileName(CPROPILE_ABTRIGGER);
	// 오버랩 이벤트에 함수 등록.
	StageTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AABStageGimmick::OnStageTriggerBeginOverlap
	);

	// Gate Section.

}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
