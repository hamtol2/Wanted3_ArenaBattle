// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// 초기값 설정.
	//MaxHp = 200.0f;
	//CurrentHp = MaxHp;
	CurrentLevel = 1;
	AttackRadius = 50.0f;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//CurrentHp = MaxHp;

	// 기본 스탯 데이터 로드.
	SetLevelStat(CurrentLevel);

	// 스탯 데이터의 최대 체력으로 설정.
	SetHp(BaseStat.MaxHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	// 레벨 값 업데이트.
	CurrentLevel = FMath::Clamp(
		InNewLevel,
		1,
		UABGameSingleton::Get().CharacterMaxLevel
	);

	// 데이터 싱글톤에 접근해서 레벨에 맞는 스탯 데이터 로드.
	BaseStat 
		= UABGameSingleton::Get().GetCharacterStat(CurrentLevel);

	// 확인.
	ensureAlways(BaseStat.MaxHp > 0.0f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 대미지 처리 계산을 위해 이전 체력 저장.
	const float PrevHp = CurrentHp;

	// 대미지 값에 음수가 전달되는 것 방지.
	// Clamp는 지정한 Min, Max 범위를 벗어나지 않도록 처리할 때 사용.
	const float ActualDamage
		= FMath::Clamp(InDamage, 0.0f, InDamage);
	//const float ActualDamage = FMath::Max(InDamage, 0.0f);

	// 대미지 처리.
	//CurrentHp = FMath::Clamp(PrevHp - ActualDamage, 0.0f, MaxHp);
	SetHp(PrevHp - ActualDamage);

	// 체력을 모두 소진했는지 확인.
	//if (CurrentHp == 0.0f)
	// KINDA_SMALL_NUMBER -> 0으로 취급하는 매우 작은 수.
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 캐릭터 죽음 처리.
		// 지금 구조에서는 하위 계층인 컴포넌트가 직접 
		// 캐릭터에 접근해서 체력이 소진됐음을 알려야 함.
		// 이를 방지하기 위해 델리게이트를 사용.
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	//CurrentHp = NewHp;
	CurrentHp = FMath::Clamp(NewHp, 0.0f, BaseStat.MaxHp);

	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);
}
