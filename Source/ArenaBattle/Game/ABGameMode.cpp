// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
//#include "Player/ABPlayerController.h"
//#include "Character/ABCharacterPlayer.h"

AABGameMode::AABGameMode()
{
	// Search a blueprint character asset.
	static ConstructorHelpers::FClassFinder<APawn> ABCharacterClassRef(
		TEXT("/Script/ArenaBattle.ABCharacterPlayer")
	);

	if (ABCharacterClassRef.Succeeded())
	{
		// Set Default Pawn Class.
		DefaultPawnClass = ABCharacterClassRef.Class;
		//DefaultPawnClass = AABCharacterPlayer::StaticClass();
	}

	// Set Default Pawn Class.
	//DefaultPawnClass

	// Set Player Controller Class.
	//PlayerControllerClass = AABPlayerController::StaticClass();
	//"/Script/CoreUObject.Class'/Script/ArenaBattle.ABPlayerController'"

	// Search a blueprint character asset.
	//static ConstructorHelpers::FClassFinder<APlayerController> ThirdPersonPlayerControllerClassRef(
	//	TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController_C")
	//);

	//if (ThirdPersonPlayerControllerClassRef.Succeeded())
	//{
	//	PlayerControllerClass = ThirdPersonPlayerControllerClassRef.Class;
	//}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(
		TEXT("/Script/ArenaBattle.ABPlayerController")
	);
	if (PlayerControllerClassRef.Succeeded())
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
