// Copyright Epic Games, Inc. All Rights Reserved.

#include "USFightingGameMode.h"

#include "Characters/AndroidProjectPlayerController.h"
#include "Characters/USFightingCharacter.h"
#include "WeaponList.h"
#include "USFightingGameState.h"
#include "CharacterShare/EditorNames.h"

#include "EngineUtils.h"
#include "LoginManagerComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"


AUSFightingGameMode::AUSFightingGameMode()
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
		PlayerControllerBPClass(
			TEXT("/Game/BlueprintClasses/BP_AndroidProjectPlayerController.BP_AndroidProjectPlayerController_C"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	LoginManagerComp = CreateDefaultSubobject<ULoginManagerComponent>(TEXT("Login Manager"));
}

void AUSFightingGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USGameState = GetGameState<AUSFightingGameState>();

	for (TActorIterator<APlayerStart> StartIt(GetWorld()); StartIt; ++StartIt)
	{
		PlayerStarts.Add(*StartIt);
	}

	for (TActorIterator<ACameraActor> CamIt(GetWorld()); CamIt; ++CamIt)
	{
		SpectatorOverviewActor = *CamIt;
		break;
	}

}

void AUSFightingGameMode::BeginPlay()
{
	Super::BeginPlay();
	check(SpectatorOverviewActor);
	USGameState->SetOverviewObservingActor(SpectatorOverviewActor);
	check(USConsts::PLAYER_MAX_NUM_SYSTEM >= WorldPlayerNumMaximum);

}

AActor* AUSFightingGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	for(auto PlayerStart: PlayerStarts)
	{
		if(PlayerStart->PlayerStartTag != USTagNames::Taken)
		{
			PlayerStart->PlayerStartTag = USTagNames::Taken;
			return PlayerStart;
		}
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AUSFightingGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                                   FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (PlayerCount >= WorldPlayerNumMaximum)
	{
		ErrorMessage = TEXT("Player num is already full");
	}
	
}

void AUSFightingGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	check(NewPlayer != nullptr);

	AUSFightingCharacter* USFighterNewPlayer = Cast<AUSFightingCharacter>(NewPlayer->GetCharacter());
	
	check(USFighterNewPlayer != nullptr);

	
	USFighterNewPlayer->SetPlayerNetIdx(PlayerCount);
	USFighterNewPlayer->SetCharacterName(FString::Printf(TEXT("Character%d"), PlayerCount));

	USFighterNewPlayer->HandWeaponToPlayer(
		AvailableWeaponList->GetWeaponClassWithIdx(PlayerCount % AvailableWeaponList->GetWeaponClassNum()));
	USFighterNewPlayer->SetPlayerIdentityColorOnServer(
		PlayerIdentityColorRange::Colors[PlayerCount % PlayerIdentityColorRange::COLOR_RANGE_NUM]);

	USFighterNewPlayer->SetPlayerLifeCount(WorldPlayerDefaultLife);
	
	USFighterNewPlayer->OnPlayerDiedOnServer.AddDynamic(this, &AUSFightingGameMode::OnPlayerDiedCallback);

	USGameState->AddFightingCharacterOnListOnServer(USFighterNewPlayer);
	PlayerCount++;

	UE_LOG(LogTemp, Log, TEXT("%s logged in"), *NewPlayer->GetName());
}

void AUSFightingGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(TimerOnPlayerDied);
}

void AUSFightingGameMode::OnPlayerDiedCallback(AUSFightingCharacter* InCharacter, ECauseOfDeath InCauseOfDeath,
                                               float RecallTime)
{

	if (InCharacter->GetCurLifeCount() > 0)
	{
		GetWorldTimerManager().SetTimer(TimerOnPlayerDied, FTimerDelegate::CreateLambda(
			                                [InCharacter]()
			                                {
				                                if (!IsValid(InCharacter)) return;

				                                constexpr float INVINCIBLE_TIME = 3.f;
				                                InCharacter->USRecallPlayerOnServer(
					                                InCharacter->GetController()->StartSpot.Get(), INVINCIBLE_TIME);
			                                }

		                                ), RecallTime, false);
	}
	else
	{
		constexpr float DESROY_TIME=5.0f;
		USGameState->RemoveFightingCharacterOnListOnServer(InCharacter);
		
		GetWorldTimerManager().SetTimer(TimerOnPlayerDied, FTimerDelegate::CreateLambda(
			                                [InCharacter]()
			                                {
			                                	if(!IsValid(InCharacter)) return;
				                                InCharacter->Destroy(true);
			                                }

		                                ), DESROY_TIME, false);
	}
}
