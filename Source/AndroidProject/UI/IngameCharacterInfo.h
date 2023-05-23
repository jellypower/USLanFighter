// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AndroidprojectCharacter.h"
#include "Blueprint/UserWidget.h"
#include "IngameCharacterInfo.generated.h"


/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UIngameCharacterInfo : public UUserWidget
{
	GENERATED_BODY()
public:
	inline static const FName NameTagTxt = FName(TEXT("NameTagTxt"));
	void SetPlayerName(FString name){ this->playerName = name; }
	void SetPlayer(AAndroidProjectCharacter* InPlayer){ player = InPlayer;}
	virtual void NativeConstruct() override;
	
private:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString playerName;

	TWeakObjectPtr<AAndroidProjectCharacter> player;

};
