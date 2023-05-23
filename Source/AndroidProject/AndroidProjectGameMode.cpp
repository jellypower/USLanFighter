// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectGameMode.h"
#include "AndroidProjectPlayerController.h"
#include "AndroidProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAndroidProjectGameMode::AAndroidProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAndroidProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

}

void AAndroidProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Log, TEXT("%s logged in"), *NewPlayer->GetName());

	
	
}
