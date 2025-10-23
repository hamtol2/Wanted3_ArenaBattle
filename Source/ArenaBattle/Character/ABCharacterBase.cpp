// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 컨트롤 데이터 애셋 검색 후 설정.
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder"));
	if (ShoulderDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Shoulder,
			ShoulderDataRef.Object
		);
	}
	
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater"));
	if (QuaterDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Quater,
			QuaterDataRef.Object
		);
	}

	// 몽타주 애셋 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack"));
	if (ComboActionMontageRef.Succeeded())
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	// 콤보 액션 데이터 애셋 설정.
	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Game/ArenaBattle/ComboAction/ABA_ComboAction.ABA_ComboAction"));
	if (ComboActionDataRef.Succeeded())
	{
		ComboActionData = ComboActionDataRef.Object;
	}

}

void AABCharacterBase::SetCharacterControlData(
	const UABCharacterControlData* InCharacterControlData)
{
	// Pawn.
	bUseControllerRotationYaw 
		= InCharacterControlData->bUseContollerRotationYaw;

	// CharacterMovement.
	GetCharacterMovement()->bOrientRotationToMovement
		= InCharacterControlData->bOrientRotationToMovement;

	GetCharacterMovement()->bUseControllerDesiredRotation
		= InCharacterControlData->bUseControllerDesiredRotation;

	GetCharacterMovement()->RotationRate
		= InCharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	// 처음 공격할 때 처리.
	// 몽타주 재생 요청.
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// 공격이 이미 재생 중일 때는 타이머를 확인해서 콤보 타이밍 확인.
	// 타이머 핸들이 유효한지 여부로 판단 가능.
	if (!ComboTimerHandle.IsValid())
	{

	}
	
}

void AABCharacterBase::ComboActionBegin()
{
	// 현재 콤보 단계를 1단계로 설정.
	CurrentCombo = 1;

	// 이동 막기 (무브먼트 모드를 None으로 설정).
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 몽타주 재생.
	// AnimInstance를 통해서 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 몽타주 재생 속도.
		const float AttackSppedRate = 1.0f;

		// 몽타주 재생 함수.
		AnimInstance->Montage_Play(ComboActionMontage, AttackSppedRate);

		// 몽타주 재생이 끝나면 이 클래스의 특정 함수를 실행.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(
			this, &AABCharacterBase::ComboActionEnd
		);

		AnimInstance->Montage_SetEndDelegate(
			OnMontageEnded, ComboActionMontage
		);
	}
}

void AABCharacterBase::ComboActionEnd(
	UAnimMontage* TargetMontage, bool Interrupted)
{
	// 어설트.
	ensureAlways(CurrentCombo != 0);

	// 콤보 데이터 리셋.
	CurrentCombo = 0;

	// 몽타주 재생 끝나면 다시 무브먼트 모드 복구.
	GetCharacterMovement()->SetMovementMode(
		EMovementMode::MOVE_Walking
	);
}
