// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAutoDetectionType : uint8
{
	None = 0,
	Radius = 1,
	RadiusDegree = 2,
};

UENUM(BlueprintType)
enum class ECauseOfDeath : uint8
{
	Fall = 0,
	HPZero = 1,
};