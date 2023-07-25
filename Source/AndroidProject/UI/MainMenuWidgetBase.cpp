// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidgetBase.h"

#include "Settings/WeaponList.h"

void UMainMenuWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	CurWeaponIdx = 0;

	WeaponNameToEquip = AvailableWeaponList->GetWeaponNameWithIdx(CurWeaponIdx);
}

void UMainMenuWidgetBase::SetNextWeaponNameToEquip()
{
	if(CurWeaponIdx >= AvailableWeaponList->GetWeaponClassNum() - 1)
	{
		CurWeaponIdx = 0;
	}
	else CurWeaponIdx++;

	WeaponNameToEquip = AvailableWeaponList->GetWeaponNameWithIdx(CurWeaponIdx);
}

void UMainMenuWidgetBase::SetPrevWeaponNameToEquip()
{

	if(CurWeaponIdx == 0)
	{
		CurWeaponIdx = AvailableWeaponList->GetWeaponClassNum() -1;
	}
	else CurWeaponIdx--;

	WeaponNameToEquip = AvailableWeaponList->GetWeaponNameWithIdx(CurWeaponIdx);
}
