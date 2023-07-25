// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MatchablePlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ANDROIDPROJECT_API AMatchablePlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	bool HostMatchingServer();

	UFUNCTION(BlueprintCallable)
	bool IsServerHosted();

	UFUNCTION(BlueprintCallable)
	void CloseMatchingServer();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentWorldIP(bool bWithPortNo = true) const;
	
	UFUNCTION(BlueprintCallable)
	int32 GetMatchingServerPortNO();

	UFUNCTION(BlueprintCallable)
	FString GetMatchingServerID();



	

private:
	
	UPROPERTY()
	class UUSLANMatcherHostServerSubsystem* ServerSubsystem;
	
};
