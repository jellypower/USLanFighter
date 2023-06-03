// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "USWeaponBase.generated.h"

UCLASS()
class ANDROIDPROJECT_API AUSWeaponBase : public AActor
{
	GENERATED_BODY()

#pragma region Unreal events
public:	
	AUSWeaponBase();
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

#pragma endregion 

#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, Category="Component")
	USkeletalMeshComponent* WeaponMeshComp;

	UPROPERTY(VisibleAnywhere, Category="Component")
	UBoxComponent* WeaponDmgBoxComp;

#pragma endregion 


#pragma region Weapon Setting
private:

	UPROPERTY(EditDefaultsOnly, Category= WeaponAnimSetting)
	TArray<UAnimMontage*> AttackMotionMontage;

	UPROPERTY(EditDefaultsOnly, Category= WeaponAnimSetting)
	TSubclassOf<class UUSCharacterAnim> WeaponAnimBP;

	
	UPROPERTY(EditDefaultsOnly, Category= WeaponAnimSetting)
	int ComboMaxNum;
	
public:

	inline UClass* GetWeaponAnimBP() const;
	
	TObjectPtr<UAnimMontage> GetPlayerAttackAnim(int idx){ return AttackMotionMontage[idx]; }

#pragma endregion 

};
