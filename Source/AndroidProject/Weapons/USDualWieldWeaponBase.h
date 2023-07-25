// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "Weapons/USWeaponBase.h"
#include "USDualWieldWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API AUSDualWieldWeaponBase : public AUSWeaponBase
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void SetActorHiddenInGame(bool bNewHidden) override;
	
	TObjectPtr<AActor> GetSubWeapon() const { return SubWeapon; }

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SubWeaponClass;
	
	UPROPERTY()
	TObjectPtr<AActor> SubWeapon;

	
};
