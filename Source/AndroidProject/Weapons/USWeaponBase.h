// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "USWeaponBase.generated.h"

UCLASS()
class ANDROIDPROJECT_API AUSWeaponBase : public AActor
{
	GENERATED_BODY()

#pragma region Unreal events
public:	
	AUSWeaponBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
#pragma endregion 

#pragma region Components

	UPROPERTY(VisibleAnywhere, Category="Component")
	USkeletalMeshComponent* WeaponMeshComp;

#pragma endregion 


#pragma region Weapon Setting
private:

	UPROPERTY(EditDefaultsOnly, Category= WeaponSetting)
	TArray<UAnimMontage*> PlayerAttackAnim;

public:
	TArray<UAnimMontage*> GetPlayerAttackAnim(){ return PlayerAttackAnim; }

#pragma endregion 

};
