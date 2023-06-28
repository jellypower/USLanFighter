// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "USAttackBlockable.generated.h"

class AUSFightingCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUSAttackBlockable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ANDROIDPROJECT_API IUSAttackBlockable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool IsTakenImpactBlockable
		(float ImpactAmount, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser, const FVector2D& AtkDir) const = 0;

	virtual bool IsTakenDmgBlockable
		(float ImpactAmount, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser, const FVector2D& AtkDir) const = 0;
	
	virtual void OnBlocked
		(float TakenImpact, const FVector2D& TakenImpactDir, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser) = 0;

};
