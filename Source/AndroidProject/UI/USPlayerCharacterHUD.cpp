// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/USPlayerCharacterHUD.h"

#include "USTouchInterfaceButton.h"
#include "Characters/USFightingCharacter.h"
#include "Settings/USFightingGameState.h"

void UUSPlayerCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUSPlayerCharacterHUD::BindPlayer(const AUSFightingCharacter* InControllingCharacter)
{
	ControllingCharacter = InControllingCharacter;
	ApplyPlayerColor(ControllingCharacter->GetPlayerIdentityColor());

	UE_LOG(LogTemp, Log, TEXT("Player Name: %s"), *ControllingCharacter->GetName());
	
	ControllingCharacter->OnIdentityColorApply.AddUniqueDynamic(this, &UUSPlayerCharacterHUD::ApplyPlayerColor);
	
	OnBindPlayer(InControllingCharacter);
}

void UUSPlayerCharacterHUD::UnBindPlayer()
{
	ControllingCharacter = nullptr;
}

FString UUSPlayerCharacterHUD::GetPlayerName() const
{
	if(!IsValid(ControllingCharacter)) return FString("NoPlayer");
	return ControllingCharacter->GetCharacterName();
}

float UUSPlayerCharacterHUD::GetCurrentHP() const
{
	if(!IsValid(ControllingCharacter)) return 0;
	return ControllingCharacter->GetCurHP();
}

float UUSPlayerCharacterHUD::GetMaxHP() const
{
	if(!IsValid(ControllingCharacter)) return 0;
	return ControllingCharacter->GetMaxHP();
}

uint8 UUSPlayerCharacterHUD::GetBoundPlayerLife() const
{
	if(!IsValid(ControllingCharacter)) return 0;
	return ControllingCharacter->GetCurLifeCount();
}

void UUSPlayerCharacterHUD::ApplyPlayerColor(FColor InColor)
{
	PlayerColor = InColor;
	OnApplyPlayerColor(InColor);
}
