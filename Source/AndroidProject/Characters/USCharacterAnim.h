// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterShare/EUSPlayerActionState.h"

#include "USCharacterAnim.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UUSCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

#pragma region Unreal Event
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;

#pragma endregion 


#pragma region Character state
protected:
	UPROPERTY(BlueprintReadOnly)
	class AUSFightingCharacter* AnimatingCharacter;


#pragma endregion 
};
