// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "InputActionValue.h"
#include "ABCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterPlayer();

	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 캐릭터 컨트롤 변경할 때 사용할 편의 함수.
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	// 컨트롤 데이터 설정.
	virtual void SetCharacterControlData(
		const class UABCharacterControlData* InCharacterControlData
	) override;

protected:

	// 캐릭터 컨트롤 변경 입력 처리 함수.
	// 토글 함수 형태로 로직 구현.
	void ChangeCharacterControl();

	// 이동 입력 처리 함수.
	void ShoulderMove(const FInputActionValue& Value);

	// 마우스 회전 입력 처리 함수.
	void ShoulderLook(const FInputActionValue& Value);

	// Quater 컨트롤일 때 이동 처리할 함수.
	void QuaterMove(const FInputActionValue& Value);

protected:

	// 컴포넌트 구성.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;
	
	// 입력 관련 섹션.
protected:
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeControlAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> QuaterMoveAction;

	// 현재 사용 중인 캐릭터 컨트롤 타입 변수.
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	ECharacterControlType CurrentCharacterControlType;
};
