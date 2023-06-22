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

	virtual void SetOwner(AActor* NewOwner) override;


#pragma endregion


#pragma region Effect
public:
	UFUNCTION(NetMulticast, Unreliable)
	void CastAttackHitEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate);

	UFUNCTION(NetMulticast, Unreliable)
	void StartSlashEffects();

	UFUNCTION(NetMulticast, Reliable)
	void EndSlashEffect();
#pragma endregion 

#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, Category="WeaponBaseSetting")
	USkeletalMeshComponent* WeaponMeshComp;

	UPROPERTY(VisibleAnywhere, Category="WeaponBaseSetting")
	UBoxComponent* WeaponDmgBoxComp;

	UPROPERTY(VisibleAnywhere, Category="WeaponBaseSetting")
	UParticleSystemComponent* TrailParticleComp;

	UPROPERTY(VisibleInstanceOnly, Category="WeaponBaseSetting")
	TArray<class USkillComponentBase*> Skills;

public:
	class USkillComponentBase* GetSkill(uint8 SkillIdx) const;
	uint8 GetTotalSkillNum() const { return Skills.Num();}

#pragma endregion


#pragma region Weapon Setting
	
public:
	uint8 GetComboMaxNum() const { return ComboMaxNum; }

	UClass* GetWeaponAnimBP() const;

	TObjectPtr<UAnimMontage> GetPlayerAttackAnim() const { return AttackMotionMontage; }

	TObjectPtr<class AUSFightingCharacter> GetOwnerAUSCharacter() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category= "WeaponBaseSetting")
	UAnimMontage* AttackMotionMontage;

	UPROPERTY(EditDefaultsOnly, Category= "WeaponBaseSetting")
	UAnimMontage* AirAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category= "WeaponBaseSetting")
	TSubclassOf<class UUSCharacterAnim> WeaponAnimBP;
	
	UPROPERTY(EditDefaultsOnly, Category = "WeaponBaseSetting")
	UParticleSystem* AttackImpactEffect;

protected:
	TObjectPtr<class AUSFightingCharacter> OwnerAUSCharacter;	
	uint8 ComboMaxNum;

#pragma endregion

#pragma region Weapon Effect

public:
	void StartAttack(float InDmg, float InImpact = 0);
	void StartGivingDmg(float InDmg);
	void StartGivingImpact(float InImpact);

	void EndAttack();


private:
	bool bIsWeaponDmgEffective = false;
	bool bIsWeaponImpactEffective = false;

	float curWeaponDmg;
	float curWeaponImpact;

	TSet<class AUSFightingCharacter*> AlreadyAttackedCharacters;
public:
	void ClearAttackedCharacter(){AlreadyAttackedCharacters.Empty();}

#pragma endregion
};
