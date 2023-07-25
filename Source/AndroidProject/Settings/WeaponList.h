// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons/USWeaponBase.h"
#include "WeaponList.generated.h"


USTRUCT()
struct FWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AUSWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FString WeaponDescription;
};

UCLASS()
class ANDROIDPROJECT_API UWeaponList : public UDataAsset
{
	GENERATED_BODY()
public:

	UClass* GetWeaponClassWithName(FName name) const
	{
		return Data.FindChecked(name).WeaponClass;
		
	}

	UClass* GetWeaponClassWithIdx(uint8 idx) const
	{
		return Data.Array()[idx].Value.WeaponClass;
	}

	FString GetWeaponNameWithIdx(uint8 idx) const
	{
		return Data.Array()[idx].Key.ToString();
	}

	uint8 GetWeaponClassNum() const
	{
		return Data.Num();
	}

	bool IsValidWeaponName(FString WeaponName) const
	{
		return Data.Contains(FName(WeaponName));
	}

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FWeaponData> Data;

};
