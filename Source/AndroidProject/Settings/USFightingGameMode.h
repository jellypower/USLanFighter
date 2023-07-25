// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterShare/EditorNames.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterShare/ShareEnums.h"
#include "GameFramework/GameMode.h"
#include "USFightingGameMode.generated.h"

class AUSFightingCharacter;
class APlayerStart;

UCLASS(minimalapi)
class AUSFightingGameMode : public AGameModeBase
{
	GENERATED_BODY()


#pragma region Unreal Events
public:
	AUSFightingGameMode();
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion


#pragma region Components

	UPROPERTY()
	TObjectPtr<class ULoginManagerComponent> LoginManagerComp;

#pragma endregion

	
protected:

	UPROPERTY(EditDefaultsOnly)
	int32 WorldPlayerNumMaximum = USConsts::PLAYER_MAX_NUM_SYSTEM;

	UPROPERTY(EditDefaultsOnly)
	uint8 WorldPlayerDefaultLife = 3;

	UPROPERTY(EditDefaultsOnly)
	class UWeaponList* AvailableWeaponList;
	
private:

	UPROPERTY()
	TObjectPtr<class AUSFightingGameState> USGameState;
	
	int32 PlayerCount = 0;
	TArray<APlayerStart*> PlayerStarts;


	UPROPERTY()
	class ACameraActor* SpectatorOverviewActor = nullptr;

	
	UFUNCTION()
	void OnPlayerDiedCallback(AUSFightingCharacter* InCharacter, ECauseOfDeath InCauseOfDeath, float RecallTime);

	FTimerHandle TimerOnPlayerDied;
};



