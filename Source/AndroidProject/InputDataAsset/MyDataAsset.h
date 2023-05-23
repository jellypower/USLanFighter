// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "MyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UMyDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAVJoystickMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* IAVScreenTouch;
	
};
