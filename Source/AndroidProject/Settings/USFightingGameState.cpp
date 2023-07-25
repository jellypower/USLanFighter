// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/USFightingGameState.h"

#include "Net/UnrealNetwork.h"
#include "Characters/USFightingCharacter.h"


void AUSFightingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUSFightingGameState, FightingCharacters);
	DOREPLIFETIME(AUSFightingGameState, OverviewObservingActor);
}

void AUSFightingGameState::AddFightingCharacterOnListOnServer(AUSFightingCharacter* InCharacter)
{
	FightingCharacters.Add(InCharacter);
}

void AUSFightingGameState::RemoveFightingCharacterOnListOnServer(AUSFightingCharacter* InCharacter)
{
	FightingCharacters.Remove(InCharacter);
}

void AUSFightingGameState::SetOverviewObservingActor(AActor* InActor)
{
	OverviewObservingActor = InActor;
}

AActor* AUSFightingGameState::GetActorToView(uint8 PlayerNetIdx) const
{
	if (FightingCharacters.IsValidIndex(PlayerNetIdx))
		return FightingCharacters[PlayerNetIdx];

	return OverviewObservingActor;
}
