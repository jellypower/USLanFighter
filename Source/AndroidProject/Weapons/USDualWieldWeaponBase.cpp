// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/USDualWieldWeaponBase.h"


void AUSDualWieldWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if(SubWeaponClass != nullptr){

		const FActorSpawnParameters params;
		
		SubWeapon = GetWorld()->SpawnActor<AActor>(*SubWeaponClass, params);
		//SubWeapon->AttachToComponent(SubWeaponContainerComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SubWeapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SubWeapon->SetActorRelativeLocation(FVector(100,0,0));
		check(IsValid(SubWeapon));
	}
}

void AUSDualWieldWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(IsValid(SubWeapon))
	{
		SubWeapon->Destroy();
	}
}

void AUSDualWieldWeaponBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	SubWeapon->SetActorHiddenInGame(bNewHidden);
}
