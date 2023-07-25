// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterShare/EditorNames.h"
#include "GameFramework/GameState.h"
#include "USFightingGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetPlayerLife, uint8, PlayerIdx, uint8, LifeCount);

UCLASS()
class ANDROIDPROJECT_API AUSFightingGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddFightingCharacterOnListOnServer(class AUSFightingCharacter* InCharacter);
	void RemoveFightingCharacterOnListOnServer(class AUSFightingCharacter* InCharacter);
	void SetOverviewObservingActor(AActor* InActor);

	uint8 GetActiveCharacterCount() const { return FightingCharacters.Num(); }

	/** Get Actor of USFightingCharacter or WorldObserverViewActor */
	AActor* GetActorToView(uint8 PlayerNetIdx) const;
private:
	
	UPROPERTY(Replicated, VisibleInstanceOnly)
	TArray<class AUSFightingCharacter*> FightingCharacters;

	UPROPERTY(Replicated)
	AActor* OverviewObservingActor;
	
};
