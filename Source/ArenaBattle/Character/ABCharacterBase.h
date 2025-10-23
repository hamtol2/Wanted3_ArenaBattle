// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

// 캐릭터 컨트롤 타입을 지정하는 열거형.
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:

	// 컨트롤 데이터 설정.
	virtual void SetCharacterControlData(
		const class UABCharacterControlData* InCharacterControlData
	);

	// Combo 섹션.
protected:
	// 콤보 액션 처리 함수.
	// 공격 처음 시작할 때와 콤보 액션을 진행할 때 실행.
	void ProcessComboCommand();

	// 콤보 액션이 시작될 때 실행.
	void ComboActionBegin();

	// 콤보 공격이 끝날 때 실행되는 함수.
	void ComboActionEnd(
		UAnimMontage* TargetMontage, bool Interrupted
	);

	// 콤보 타이머 설정 함수.
	void SetComboCheckTimer();

	// 콤보 타이밍 처리 함수.
	// 타이머에 설정된 시간이 경과했을 때 실행됨.
	// 이 시간 전에 공격 입력이 제대로 들어왔는지 판단 후 콤보 처리 분기.
	void ComboCheck();

protected:

	// 열거형 타입-UABCharacterControlData 애셋을 짝(Pair)로 관리하는 맵.
	UPROPERTY(EditAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

	// 콤보 공격 애님 몽타주.
	UPROPERTY(EditAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	// 콤보 처리를 위한 데이터를 가지는 데이터 애셋.
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	// 현재 콤보 단계 추적 변수.
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	uint32 CurrentCombo = 0;

	// 콤보 판정에 사용할 타이머.
	FTimerHandle ComboTimerHandle;

	// 콤보 점프를 판정할 때 사용할 Bool 변수.
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	bool HasNextComboCommand = false;
};
