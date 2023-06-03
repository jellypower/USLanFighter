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

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FWeaponData> Data;

};
