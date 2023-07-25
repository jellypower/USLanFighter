// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USPlayerCharacterHUD.generated.h"

/**
 * 
 */


UCLASS()
class ANDROIDPROJECT_API UUSPlayerCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:

	void BindPlayer(const class AUSFightingCharacter* InControllingCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBindPlayer(const class AUSFightingCharacter* InControllingCharacter);


	void UnBindPlayer();

protected:
	UPROPERTY(BlueprintReadOnly)
	FColor PlayerColor;

	UFUNCTION(BlueprintPure)
	FString GetPlayerName() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentHP() const; 

	UFUNCTION(BlueprintPure)
	float GetMaxHP() const;

	UFUNCTION(BlueprintPure)
	uint8 GetBoundPlayerLife() const;

	UFUNCTION()
	void ApplyPlayerColor(FColor InColor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnApplyPlayerColor(FColor InColor);

private:

	UPROPERTY()
	const class AUSFightingCharacter* ControllingCharacter;

};
