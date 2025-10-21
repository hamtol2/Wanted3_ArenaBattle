// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "ABCharacterControlData.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 컴포넌트 설정.
	// 컨트롤러 회전 값을 받아서 사용하는 옵션 비활성화.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 무브먼트 설정.
	// 이동할 때 이동하는 방향과 회전 방향을 맞추는 옵션 설정.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 애셋 지정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharaterAnim(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C"));
	if (CharaterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharaterAnim.Class);
	}

	// SpringArm 컴포넌트 생성 및 설정.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Camera 컴포넌트 생성 및 설정.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 입력 설정.
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulerMove.IA_ShoulerMove"));
	if (MoveActionRef.Succeeded())
	{
		ShoulderMoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook"));
	if (LookActionRef.Succeeded())
	{
		ShoulderLookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> QuaterMoveActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove"));
	if (QuaterMoveActionRef.Succeeded())
	{
		QuaterMoveAction = QuaterMoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeControlActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (ChangeControlActionRef.Succeeded())
	{
		ChangeControlAction = ChangeControlActionRef.Object;
	}

	// 초기 캐릭터 컨트롤 타입 설정.
	CurrentChracterControlType = ECharacterControlType::Quater;
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 초기 입력 컨트롤 설정.
	SetCharacterControl(CurrentChracterControlType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 향상된 입력 시스템을 처리하는 컴포넌트로 형변환.
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 바인딩(연결).
		EnhancedInputComponent->BindAction(
			ShoulderMoveAction,
			ETriggerEvent::Triggered,
			this,
			&AABCharacterPlayer::ShoulderMove
		);

		EnhancedInputComponent->BindAction(
			ShoulderLookAction,
			ETriggerEvent::Triggered,
			this,
			&AABCharacterPlayer::ShoulderLook
		);

		EnhancedInputComponent->BindAction(
			QuaterMoveAction,
			ETriggerEvent::Triggered,
			this,
			&AABCharacterPlayer::QuaterMove
		);

		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Started,
			this,
			&ACharacter::Jump
		);

		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Completed,
			this,
			&ACharacter::StopJumping
		);

		EnhancedInputComponent->BindAction(
			ChangeControlAction,
			ETriggerEvent::Started,
			this,
			&AABCharacterPlayer::ChangeCharacterControl
		);
	}
}

// 캐릭터 컨트롤 변경할 때 사용할 편의 함수.
void AABCharacterPlayer::SetCharacterControl(
	ECharacterControlType NewCharacterControlType)
{
	// 변경할 컨트롤 데이터 애셋 로드.
	UABCharacterControlData* NewCharacterControl
		= CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	// 데이터 애셋을 전달해 함수 호출.
	SetCharacterControlData(NewCharacterControl);

	// 입력 매핑 컨텍스트 관련 처리.
	// 향상된 입력 시스템 객체 얻어오기.
	APlayerController* PlayerController
		= Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()
		);

		// 사용할 입력 매핑 컨텍스트 등록.
		if (InputSystem)
		{
			// 기존에 설정된 매핑 제거.
			InputSystem->ClearAllMappings();

			// 새로운 입력 매핑 컨텍스트 추가.
			InputSystem->AddMappingContext(
				NewCharacterControl->InputMappingContext,
				0
			);
		}
	}
}

// 컨트롤 데이터 설정.
void AABCharacterPlayer::SetCharacterControlData(
	const UABCharacterControlData* InCharacterControlData)
{
	Super::SetCharacterControlData(InCharacterControlData);

	// SpringArm 관련 설정.
	SpringArm->TargetArmLength
		= InCharacterControlData->TargetArmLength;

	SpringArm->SetRelativeRotation(
		InCharacterControlData->RelativeRotation
	);

	SpringArm->bDoCollisionTest 
		= InCharacterControlData->bDoCollisionTest;

	SpringArm->bUsePawnControlRotation
		= InCharacterControlData->bUsePawnControlRotation;

	SpringArm->bInheritPitch
		= InCharacterControlData->bInheritPitch;

	SpringArm->bInheritYaw 
		= InCharacterControlData->bInheritYaw;
	
	SpringArm->bInheritRoll
		= InCharacterControlData->bInheritRoll;
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	// 사용할 캐릭터 컨트롤을 변경.
	// 조건이 두개일때 if/else 보다는 if/else if로 확인하고, else는 예외처리로.
	if (CurrentChracterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentChracterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
	//else
	//{
	//	// 예외처리.
	//}
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	// 입력 값 읽어오기.
	FVector2D Movement = Value.Get<FVector2D>();

	// 이동할 방향 만들기.
	// 카메라가 바라보는 방향(컨트롤러의 방향)을 기준으로 이동 방향 만들기.
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// 앞방향.
	FVector ForwardVector 
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// 오른쪽 방향.
	FVector RightVector 
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 무브먼트에 입력 전달하기.
	// 크기가 1인 숫자를 곱하는 행위는 "회전"
	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	// 입력 값 읽어오기.
	FVector2D LookValue = Value.Get<FVector2D>();

	// 컨트롤러에 회전 적용.

	// 마우스 좌우 드래그 입력을 컨트롤러의 Z축 회전(요, Yaw)에 적용.
	AddControllerYawInput(LookValue.X);

	// 마우스 좌우 드래그 입력을 컨트롤러의 Y축 회전(피치, Pitch)에 적용.
	AddControllerPitchInput(LookValue.Y);
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	// 쿼터뷰 일 때 이동로직 처리.

}
