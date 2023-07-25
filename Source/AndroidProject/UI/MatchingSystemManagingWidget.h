// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchingSystemManagingWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UMatchingSystemManagingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	bool HostMatchingServer();


	UFUNCTION(BlueprintPure)
	bool IsServerHosted();

	UFUNCTION(BlueprintCallable)
	void ProcessJoinRequests();

	UFUNCTION(BlueprintCallable)
	void CloseMatchingServer();

	UFUNCTION(BlueprintPure)
	FString GetCurrentWorldIP(bool bWithPortNo = true) const;

	UFUNCTION(BlueprintPure)
	int32 GetMatchingServerPortNO() const;

	UFUNCTION(BlueprintPure)
	FString GetMatchingServerID() const;



	UFUNCTION(BlueprintCallable)
	void TravelToBattleField(FString InWorldName);
	
private:
	UPROPERTY()
	class UUSLANMatcherHostServerSubsystem* ServerSubsystem;
};
