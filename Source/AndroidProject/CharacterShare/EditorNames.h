// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NameTypes.h"


namespace USSocketNames
{
	const static FName Shield = FName("shield_l");
	const static FName Weapon = FName("weapon_r");
	const static FName Weapon_Socket_slash_start = FName("Slash_Start");
	const static FName Weapon_Socket_slash_end = FName("Slash_End");
}

namespace USMaterialParamNames
{
	const static FName Tint=FName("Tint");
	const static FName Brightness=FName("Metal_Brightness");
}

namespace USTagNames
{
	const static FName Taken = FName("Taken");
	const static FName SpectatorStart = FName("SpectatorStart");
}

namespace CollisionName
{
	const static FName NoCollision = FName("NoCollision");
	const static FName Attack = FName("Attack");
	const static FName Spectator = FName("Spectator");
	const static FName Pawn = FName("Pawn");
}

namespace USCollisionChannel
{
	constexpr static ECollisionChannel Attack = ECollisionChannel::ECC_GameTraceChannel1;
	constexpr static ECollisionChannel DmgBoxChannel = ECollisionChannel::ECC_GameTraceChannel2;
}

namespace USConsts
{
	constexpr float LAUNCH_COEFF = 50.f;
	constexpr float BLOWN_THRESHOLD = 1.f + FLT_EPSILON;

	constexpr uint8 BLINKING_MATERIAL_IDX = 0;
	constexpr uint8 IDENTITY_MATERIAL_IDX = 1;

	constexpr uint8 PLAYER_MAX_NUM_SYSTEM = 4;
}

namespace USF_BP_Consts
{
	
}

namespace PlayerIdentityColorRange
{
	constexpr uint8 COLOR_RANGE_NUM = 5;
	
	const static FColor Colors[COLOR_RANGE_NUM]
	{
		FColor(1,0,0),
		FColor(0,1,0),
		FColor(0,0,1),
		FColor(0,0,0),
		FColor(1,1,1)
	};
}

namespace MontageSectionNames
{
	const static FName Combo[4]
	{
		FName("Combo1"),
		FName("Combo2"),
		FName("Combo3"),
		FName("Combo4")
	};

}

