// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


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
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
}
