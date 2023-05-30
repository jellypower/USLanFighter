// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EUSPlayerActionState : uint8
{
	None = 0 UMETA(Hidden),
	
	Move = 0x1,
	Jump = 0x1 << 1,
	Attack = 0x1 << 2,
	Impacted = 0x1 << 3,
	Blown = 0x1 << 4,

	Max = 0xFF UMETA(Hidden)
};

inline uint8 operator|(const uint8& lval, const EUSPlayerActionState& rval)
{
	return lval | static_cast<uint8>(rval);
}

inline uint8 operator|(const EUSPlayerActionState& lval, const uint8& rval)
{
	return static_cast<uint8>(lval) | rval;
}

inline uint8 operator|(const EUSPlayerActionState& lval, const EUSPlayerActionState& rval)
{
	return static_cast<uint8>(lval) | static_cast<uint8>(rval);
}

inline uint8 operator&(const uint8& lval, const EUSPlayerActionState& rval)
{

	return lval & static_cast<uint8>(rval);
}

inline uint8 operator&(const EUSPlayerActionState& lval, const uint8& rval)
{
	return static_cast<uint8>(lval) & rval;
}

inline uint8 operator&(const EUSPlayerActionState& lval, const EUSPlayerActionState& rval)
{
	return static_cast<uint8>(lval) & static_cast<uint8>(rval);
}

inline uint8 operator~(const EUSPlayerActionState& val)
{
	return ~static_cast<uint8>(val);
}


namespace EUSPlayerActionStateValue
{
	static uint8 OrderUnAcceptableBitMask =
		EUSPlayerActionState::Impacted |
		EUSPlayerActionState::Blown |
		EUSPlayerActionState::Jump; // TO-DO: delete jump from here and create new bitmask value
}

