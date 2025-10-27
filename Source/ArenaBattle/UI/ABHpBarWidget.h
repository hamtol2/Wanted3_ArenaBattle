// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

	// 최대 체력 값 설정 함수.
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }

	// HpBar에 게이지 설정을 위해 사용할 함수.
	void UpdateHpBar(float NewCurrentHp);

protected:
	// 위젯이 모두 초기화가 된 후에 호출되는 함수.
	// 위젯을 참조할 때 생성됐다는 것을 보장 받을 수 있음.
	virtual void NativeConstruct() override;

protected:

	// ProgressBar 참조 변수.
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	// @Temp, @InComplete.
	// 최대 체력 값 (게이지 계산용).
	UPROPERTY()
	float MaxHp;
};
