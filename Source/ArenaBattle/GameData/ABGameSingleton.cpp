// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/ABGameSingleton.h"

DEFINE_LOG_CATEGORY(LogABGameSingleton);

UABGameSingleton::UABGameSingleton()
{
	// 데이터 테이블 애셋 로드.
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Game/ArenaBattle/GameData/ABCharacterStatTable.ABCharacterStatTable"));

	if (DataTableRef.Succeeded())
	{
		// 데이터 테이블은 키/값 조합으로 구성됨(내부 컨테이너가 TMap).
		const UDataTable* DataTable = DataTableRef.Object;

		// 확인.
		ensureAlways(DataTable->GetRowMap().Num() > 0);

		// 편의를 위해 배열로 변환해서 처리.
		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);

		// 형변환해서 배열에 추가.
		for (int32 Index = 0; Index < ValueArray.Num(); ++Index)
		{
			CharacterStatTable.Add(
				*reinterpret_cast<FABCharacterStat*>(ValueArray[Index])
			);
		}

		// 위의 코드와 같은 일 처리를 함.
		//Algo::Transform(ValueArray, CharacterStatTable, [](uint8* Value)
		//	{
		//		return *reinterpret_cast<FABCharacterStat*>(Value);
		//	});

		// 스탯 테이블의 항목 수를 최대 레벨 값으로 설정.
		CharacterMaxLevel = CharacterStatTable.Num();

		// 확인.
		ensureAlways(CharacterMaxLevel > 0);
	}

}

UABGameSingleton& UABGameSingleton::Get()
{
	// 싱글톤 로드.
	UABGameSingleton* Singleton
		= Cast<UABGameSingleton>(GEngine->GameSingleton);

	if (Singleton)
	{
		return *Singleton;
	}

	// 오류.
	UE_LOG(LogABGameSingleton, Error, TEXT("Invalid Game Singleton"));

	// 이 코드 실행되면 안됨.
	return *NewObject<UABGameSingleton>();
}
