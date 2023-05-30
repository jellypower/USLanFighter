// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectGameMode.h"
#include "Characters/AndroidProjectPlayerController.h"
#include "Characters/USFightingCharacter.h"
#include "UObject/ConstructorHelpers.h"


AAndroidProjectGameMode::AAndroidProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAndroidProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn>
		PlayerPawnBPClass(TEXT("/Game/BlueprintClasses/BP_USFightCharacter.BP_USFightCharacter_C"));
	if (PlayerPawnBPClass.Class != nullptr)
	{	
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController>
		PlayerControllerBPClass(TEXT("/Game/BlueprintClasses/BP_AndroidProjectPlayerController.BP_AndroidProjectPlayerController_C"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

}

void AAndroidProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	check(NewPlayer != nullptr);

	check(Cast<AUSFightingCharacter>(NewPlayer->GetCharacter()) != nullptr);
	
	Cast<AUSFightingCharacter>(NewPlayer->GetCharacter())
		->SetCharacterName(FString::Printf(TEXT("Character%d"), PlayerCount));

	PlayerCount++;
	
	UE_LOG(LogTemp, Log, TEXT("%s logged in"), *NewPlayer->GetName());

	
	
}
