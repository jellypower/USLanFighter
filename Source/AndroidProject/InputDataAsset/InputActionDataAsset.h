// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputActionDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UInputActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAMove;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IASkill1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IASkill2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAKick;
	
};
