// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "USOrder.generated.h"

UENUM(BlueprintType)
enum class FUSOrderType : uint8
{
	None = 0,
	Move,
	Jump,
	Attack,
	Skill1,
	Skill2,
	Smash
};


USTRUCT(BlueprintType)
struct FUSOrder
{
	GENERATED_BODY()

	FUSOrder() :
		Type(FUSOrderType::None), dir(FVector_NetQuantizeNormal::Zero())
	{
	}

	FUSOrder(FUSOrderType InType) :
		Type(InType), dir(FVector_NetQuantizeNormal::Zero())
	{
	}

	FUSOrder(FUSOrderType InType, FVector_NetQuantizeNormal InDir) :
		Type(InType), dir(InDir)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUSOrderType Type;

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal dir;


	void Init()
	{
		dir = FVector_NetQuantizeNormal::Zero();
		Type = FUSOrderType::None;
	}

	bool IsMovementOrder() const
	{
		return
			Type == FUSOrderType::Move ||
			Type == FUSOrderType::Jump;
	}

	bool IsCastOrder() const
	{
		return
			Type == FUSOrderType::Attack ||
			Type == FUSOrderType::Skill1 ||
			Type == FUSOrderType::Skill2 ||
			Type == FUSOrderType::Smash;
	}
};
