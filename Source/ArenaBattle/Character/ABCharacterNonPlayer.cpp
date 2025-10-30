// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"
#include "CharacterStat/ABCharacterStatComponent.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	// 시작할 때는 메시가 안 보이도록 설정.
	GetMesh()->SetHiddenInGame(true);

	// AIController 설정.
	AIControllerClass = AABAIController::StaticClass();

	// 맵에서 배치하거나 동적으로 생성될 때 
	// 모두 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 확인.
	ensureAlways(NPCMeshes.Num() > 0);

	// 랜덤으로 배열 인덱스 선택.
	int32 RandomIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);

	// 비동기 로딩.
	NPCMeshHandle
		= UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			NPCMeshes[RandomIndex],
			FStreamableDelegate::CreateUObject(
				this,
				&AABCharacterNonPlayer::NPCMeshLoadCompleted
			)
		);
}

void AABCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	// 시간 대기에 사용될 타이머 핸들.
	FTimerHandle DeadTimerHandle;

	// 설정한 시간이 경과했을 때 실행할 델리게이트.
	auto Delegate = FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	);

	// 타이머를 사용해서 일정 시간 이후에 삭제 처리.
	GetWorld()->GetTimerManager().SetTimer(
		DeadTimerHandle,
		Delegate,
		DeadEventDelayTime,
		false
	);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	// 애셋 로드가 끝났을 때 핸들이 유효한지 확인.
	if (NPCMeshHandle.IsValid())
	{
		// 로드된 애셋을 스켈레탈 메시로 형변환.
		USkeletalMesh* NPCMesh
			= Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());

		if (NPCMesh)
		{
			// 스켈레탈 메시 설정.
			GetMesh()->SetSkeletalMesh(NPCMesh);

			// 감춰뒀던 메시 컴포넌트를 다시 보이게 설정.
			GetMesh()->SetHiddenInGame(false);
		}
	}

	// 모든 작업이 끝난 후 핸들 해제.
	NPCMeshHandle->ReleaseHandle();
}

float AABCharacterNonPlayer::GetAIPatrolRadius()
{
	return 500.0f;
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	// 캡슐 길이 구하는 공식.
	return Stat->GetTotalStat().AttackRange 
		+ Stat->GetAttackRadius() * 2;
}

float AABCharacterNonPlayer::GetAIAITurnSpeed()
{
	return 0.0f;
}
