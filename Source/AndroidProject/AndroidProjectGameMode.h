// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AndroidProjectGameMode.generated.h"

UCLASS(minimalapi)
class AAndroidProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAndroidProjectGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



