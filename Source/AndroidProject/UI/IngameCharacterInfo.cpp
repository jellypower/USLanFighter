// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameCharacterInfo.h"

#include "AndroidProject/AndroidProjectCharacter.h"

void UIngameCharacterInfo::NativeConstruct()
{
	Super::NativeConstruct();

	playerName = player->GetCharacterName();
	UE_LOG(LogTemp, Log, TEXT("Native Construct"));

}



