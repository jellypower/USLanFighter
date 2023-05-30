// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameCharacterInfo.h"
#include "Characters/USFightingCharacter.h"



void UIngameCharacterInfo::NativeConstruct()
{
	Super::NativeConstruct();

	checkf(player != nullptr, TEXT("Character info widget muset to be SetPlayer"));
	
	playerName = player->GetCharacterName();
}

float UIngameCharacterInfo::GetCurrentHP() const
{
	return player->GetCurHP();
}

float UIngameCharacterInfo::GetMaxHP() const
{
	return player->GetMaxHP(); 
}

void UIngameCharacterInfo::SetPlayer(AUSFightingCharacter* InPlayer)
{
	player = InPlayer; 
}



