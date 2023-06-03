// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AndroidProjectGameMode.generated.h"

UCLASS(minimalapi)
class AAndroidProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

#pragma region Unreal Events
public:
	AAndroidProjectGameMode();
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

#pragma endregion


#pragma region Components

	UPROPERTY()
	TObjectPtr<class ULoginManagerComponent> LoginManagerComp;

#pragma endregion

	
private:
	int32 PlayerCount = 0;
};



