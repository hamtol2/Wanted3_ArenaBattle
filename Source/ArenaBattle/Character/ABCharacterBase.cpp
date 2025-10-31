// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABComboActionData.h"
#include "Physics/ABCollision.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABWidgetComponent.h"

#include "UI/ABHpBarWidget.h"
#include "Item/ABItemData.h"
//#include "Item/ABWeaponItemData.h"
#include "Item/ABItems.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 설정.
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	GetCapsuleComponent()->SetCollisionProfileName(CPROPILE_ABCAPSULE);

	// 메시의 콜리전은 NoCollision으로 설정.
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 애셋 지정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Warrior.SK_CharM_Warrior"));
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharaterAnim(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (CharaterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharaterAnim.Class);
	}


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

	// 죽음 몽타주 애셋 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Game/ArenaBattle/Animation/AM_Dead.AM_Dead"));
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Stat Component.
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	// Widget Component.
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());

	// 위치 조정.
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	// 어떤 WidgetBlueprint를 사용해 그릴지 지정.
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HPBar.WBP_HPBar_C"));
	if (HpBarWidgetRef.Succeeded())
	{
		// 컴포넌트에서 사용할 위젯 클래스 설정.
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);

		// 위젯을 그릴 공간 지정.
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);

		// 그릴 크기 지정.
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));

		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Item 섹션.
	TakeItemActions.Add(
		FTakeItemDelegateWrapper(
			FOnTakeItemDelegate::CreateUObject(
				this,
				&AABCharacterBase::EquipWeapon)
		)
	);

	TakeItemActions.Add(
		FTakeItemDelegateWrapper(
			FOnTakeItemDelegate::CreateUObject(
				this,
				&AABCharacterBase::DrinkPotion)
		)
	);

	TakeItemActions.Add(
		FTakeItemDelegateWrapper(
			FOnTakeItemDelegate::CreateUObject(
				this,
				&AABCharacterBase::ReadScroll)
		)
	);

	// Weapon Component.
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 체력을 모두 소진했을 때 발행되는 델리게이트에 구독.
	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);

	// 스탯이 변경됐을 때 발행되는 델리게이트에 구독.
	Stat->OnStatChanged.AddUObject(
		this, 
		&AABCharacterBase::ApplyStat
	);
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

float AABCharacterBase::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	/*float AppliedDamage = */Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser
	);

	// @Test: 바로 죽음 처리.
	//SetDead();
	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
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
	//HasNextComboCommand = ComboTimerHandle.IsValid();
	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
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
		//const float AttackSpeedRate = 1.0f;
		const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;

		// 몽타주 재생 함수.
		AnimInstance->Montage_Play(
			ComboActionMontage, AttackSpeedRate
		);

		// 몽타주 재생이 끝나면 이 클래스의 특정 함수를 실행.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(
			this, &AABCharacterBase::ComboActionEnd
		);

		AnimInstance->Montage_SetEndDelegate(
			OnMontageEnded, ComboActionMontage
		);

		// 콤보 타이밍 확인용 타이머 설정.
		// 기존에 설정된 타이머 핸들 무효화(초기화).
		ComboTimerHandle.Invalidate();

		// 타이머 설정 및 콤보 단계 처리.
		SetComboCheckTimer();
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

	// 공격이 끝나면 NotifyComboActionEnd 함수 호출.
	NotifyComboActionEnd();
}

void AABCharacterBase::NotifyComboActionEnd()
{
}

void AABCharacterBase::SetComboCheckTimer()
{
	// 현재 재생중인 콤보의 인덱스 계산.
	int32 ComboIndex = CurrentCombo - 1;

	// 계산된 인덱스 값 검증(어설트).
	ensureAlways(
		ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex)
	);

	// 데이터 애셋에 설정된 프레임 값을 사용해 
	// 콤보 판정 구간의 시간 값(단위: 초) 구하기.
	// 왜? 타이머에 쓰려고.

	// 애니메이션 재생 속도.
	//const float AttackSpeedRate = 1.0f;
	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	// 초 단위 시간 값 계산 (타이머에 설정할 값).
	float ComboEffectTime =
		(ComboActionData->EffectiveFrameCount[ComboIndex]
			/ ComboActionData->FrameRate) / AttackSpeedRate;

	// 타이머 설정.
	if (ComboEffectTime > 0)
	{
		// 시간은 월드가 관리(시간 관리자를 통해)
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AABCharacterBase::ComboCheck,
			ComboEffectTime,
			false
		);
	}

}

void AABCharacterBase::ComboCheck()
{
	// 타이머 핸들 초기화(재사용을 위해).
	ComboTimerHandle.Invalidate();

	// 콤보 타이머 시간 전에 공격 입력이 들어왔는지 확인.
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance
			= GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 몽타주 섹션 점프.
		// "ComboAttack1" -> "ComboAttack2".
		// "접두어" + (CurrentCombo + 1) -> ComboAttack2..
		//CurrentCombo = CurrentCombo + 1;
		//if (CurrentCombo > ComboActionData->EffectiveFrameCount.Num())
		//{
		//	CurrentCombo = ComboActionData->EffectiveFrameCount.Num();
		//}
			CurrentCombo = FMath::Clamp(
				CurrentCombo + 1,
				1,
				ComboActionData->MaxComboCount
			);

			// 섹션 이름 만들기.
			FName NextSecion = *FString::Printf(
				TEXT("%s%d"),
				*ComboActionData->MontageSectionNamePrefix,
				CurrentCombo
			);

			// 몽타주 점프.
			AnimInstance->Montage_JumpToSection(
				NextSecion,
				ComboActionMontage
			);

			// 타이머 재설정.
			SetComboCheckTimer();
			// 콤보 처리에 사용한 입력 값도 초기화.
			HasNextComboCommand = false;
		}
	}
}

void AABCharacterBase::SetDead()
{
	// 캐릭터 무브먼트 끄기.
	GetCharacterMovement()->SetMovementMode(
		EMovementMode::MOVE_None
	);

	// 죽는 애니메이션 재생.
	PlayDeadAnimation();

	// 콜리전 끄기.
	SetActorEnableCollision(false);

	// 죽으면 HpBar 사라지도록 처리.
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	// 몽타주 재생.
	UAnimInstance* AnimInstance
		= GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 이미 재생 중인 몽타주가 있을 수 있으니 모두 종료.
		AnimInstance->StopAllMontages(0.0f);

		// 재생 속도.
		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(
			DeadMontage,
			PlayRate
		);
	}
}



void AABCharacterBase::AttackHitCheck()
{
	// 애님 노티파이를 통해 함수가 호출됨.
	// 충돌 판정 로직 작성.

	// 충돌 판정 시작 위치.
	FVector Start
		= GetActorLocation()
		+ GetActorForwardVector()
		* GetCapsuleComponent()->GetScaledCapsuleRadius();

	// 충돌 판정 종료 위치.
	//const float AttackRange = 50.0f;
	const float AttackRange = Stat->GetTotalStat().AttackRange;
	FVector End
		= Start + GetActorForwardVector() * AttackRange;

	// 트레이스에 사용할 반지름 값.
	//const float AttackRadius = Stat->GetAttackRadius();
	const float AttackRadius = Stat->GetAttackRadius();

	// SCENE_QUERY_STAT-FName 타입의 태그값 생성 매크로(엔진 내부에서 사용).
	// 두번째 인자: 복잡한 형태로 충돌체를 감지할지 여부 지정.
	// 세번째 인자: 충돌 판정에서 제외할 액터 목록 (자기자신 제외).
	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(Attack),
		false,
		this
	);

	// 트레이스를 활용해 충돌 검사.
	FHitResult OutHitResult;
	bool HitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	// 충돌이 감지된 경우의 처리.
	if (HitDetected)
	{
		// 대미지 양.
		//const float AttackDamage = 30.0f;
		const float AttackDamage = Stat->GetTotalStat().Attack;

		// 대미지 이벤트.
		FDamageEvent DamageEvent;

		// 대미지 전달.
		OutHitResult.GetActor()->TakeDamage(
			AttackDamage,
			DamageEvent,
			GetController(),
			this
		);
	}

	// 디버그 모드일 때만 그리도록.
#if ENABLE_DRAW_DEBUG

	// 캡슐의 중심 위치.
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;

	// 캡슐 높이의 절반 값.
	float CapsuleHalfHeight = AttackRange * 0.5f;

	// 색상 (그리기 색상).
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	// 충돌 디버그 (시각적 도구 활용).
	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		AttackRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor,
		false,
		5.0f
	);
#endif

}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	// 초기 값 설정 및 델리게이트 연결.
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 스탯 데이터를 기반으로 위젯에 값 설정.
		//HpBarWidget->SetMaxHp(Stat->GetTotalStat().MaxHp);
		HpBarWidget->UpdateStat(
			Stat->GetBaseStat(),
			Stat->GetModifierStat()
		);
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());

		// 델리게이트 연결.
		Stat->OnHpChanged.AddUObject(
			HpBarWidget,
			&UABHpBarWidget::UpdateHpBar
		);

		Stat->OnStatChanged.AddUObject(
			HpBarWidget,
			&UABHpBarWidget::UpdateStat
		);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	//UE_LOG(
	//	LogTemp, 
	//	Log, 
	//	TEXT("Item Collected. Type: %d"), 
	//	(uint8)InItemData->Type
	//);

	// 3가지 종류의 아이템에 따라 처리를 분기.
	if (InItemData)
	{
		// 타입 별로 인덱스 구하기.
		uint8 Index = (uint8)InItemData->Type;

		// 호출할 델리게이트 가져오기.
		FOnTakeItemDelegate Delegate
			= TakeItemActions[Index].ItemDelegate;

		// 델리게이트 호출.
		Delegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("Drink Potion"));

	// 처리를 위해 포션 아이템 데이터로 변환 후 체력 회복 요청.
	UABPotionItemData* PotionItemData
		= Cast<UABPotionItemData>(InItemData);
	if (PotionItemData)
	{
		Stat->HealHp(PotionItemData->HealAmount);
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("Equip Weapon"));

	// 무기 아이템 타입으로 형변환 후 무기 메시 설정.
	UABWeaponItemData* WeaponItemData
		= Cast<UABWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		// 무기 메시가 이미 로드됐는지 확인.
		// 로드가 안됐으면 로드 처리.
		if (WeaponItemData->WeaponMesh.IsPending())
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}

		// 로드가 완료되면 메시 설정.
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());

		// 무기 아이템이 제공하는 부가 스탯 설정.
		Stat->SetModifierStat(WeaponItemData->ModifierStat);
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("Read Scroll"));

	// 스크롤 아이템 타입으로 형변환 후 처리 요청.
	UABScrollItemData* ScollItemData
		= Cast<UABScrollItemData>(InItemData);
	if (ScollItemData)
	{
		Stat->AddBaseStat(ScollItemData->BaseStat);
	}
}

int AABCharacterBase::GetLevel() const
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(
	const FABCharacterStat& BaseStat, 
	const FABCharacterStat& ModifierStat)
{
	// 스탯 데이터에서 최종 이동 속력 구하기.
	float MovementSpeed
		= (BaseStat + ModifierStat).MovementSpeed;

	// 이동 속력을 캐릭터 무브먼트 컴포넌트에 적용.
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}
