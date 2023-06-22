// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
namespace EUSPlayerActionState 
{
	enum State : int
	{
		None = 0 UMETA(Hidden),

		Move = 0x1,
		Jump = 0x1 << 1,
		Cast = 0x1 << 2,
		Impacted = 0x1 << 3,
		Blown = 0x1 << 4,

		Max = -1 UMETA(Hidden)
	};
}


namespace EUSPlayerActionStateValue
{
	static uint8 OrderUnAcceptableBitMask =
		EUSPlayerActionState::Impacted |
		EUSPlayerActionState::Blown |
		EUSPlayerActionState::Jump |
		EUSPlayerActionState::Cast; // TO-DO: delete jump from here and create new bitmask value
	
}
