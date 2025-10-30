// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UABHUDWidget(
		const FObjectInitializer& ObjectInitializer
	);

	// 캐릭터에서 UI로 접근할 때 사용할 함수.
	// HpBar에 게이지 설정을 위해 사용할 함수.
	void UpdateHpBar(float NewCurrentHp);

	// 스탯 업데이트 함수.
	void UpdateStat(
		const FABCharacterStat& BaseStat,
		const FABCharacterStat& ModifierStat
	);

protected:
	virtual void NativeConstruct() override;

protected:

	// HpBar 위젯.
	UPROPERTY()
	TObjectPtr<class UABHpBarWidget> HpBar;

	// CharacterStat 위젯.
	UPROPERTY()
	TObjectPtr<class UABCharacterStatWidget> CharacterStat;
};
