// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
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
