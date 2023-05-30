// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameCharacterInfo.generated.h"



class AUSFightingCharacter;

UCLASS()
class ANDROIDPROJECT_API UIngameCharacterInfo : public UUserWidget
{
	GENERATED_BODY()

	
public:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<AUSFightingCharacter> player;
	
protected:
	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() const; 

	UFUNCTION(BlueprintCallable)
	float GetMaxHP() const; 

	UPROPERTY(BlueprintReadOnly)
	FString playerName;

public:
	inline void SetPlayer(AUSFightingCharacter* InPlayer);
	
};
