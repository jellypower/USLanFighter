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


namespace CollisionName
{
	const static FName NoCollision = FName("NoCollision");
	const static FName Attack = FName("Attack");
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
\
}

