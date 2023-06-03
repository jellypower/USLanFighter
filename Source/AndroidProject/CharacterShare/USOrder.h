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
	Kick
};


USTRUCT(BlueprintType)
struct FUSOrder
{
	GENERATED_BODY()

	FUSOrder() :
		Type(FUSOrderType::None), dir(FVector_NetQuantizeNormal::Zero())
	{}
	
	FUSOrder(FUSOrderType InType, FVector_NetQuantizeNormal InDir) :
		Type(InType), dir(InDir)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUSOrderType Type;

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal dir;

	
	
	void Init()
	{
		dir = FVector_NetQuantizeNormal::Zero();
		Type = FUSOrderType::None;
	}
	
};
