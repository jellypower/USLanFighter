// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/USCharacterAnim.h"

#include "USFightingCharacter.h"
#include "Net/Core/Connection/NetResult.h"

void UUSCharacterAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AnimatingCharacter = Cast<AUSFightingCharacter>(GetOwningActor());

}

void UUSCharacterAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(AnimatingCharacter != nullptr);
}

