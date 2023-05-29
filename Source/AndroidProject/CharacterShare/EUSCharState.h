// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class PlayerActionState : uint8
{
	Locomotion,
	Attack,
	Impacted,
	Blown
};

namespace USSocketNames
{
	const static FName LHandSocketName = FName("LHand");
	
};