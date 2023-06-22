// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponentBase.h"
#include "Dashskill.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UDashskill final : public USkillComponentBase
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;
	virtual void TriggerEffect() override;
	virtual bool IsCasting() const override;
	virtual bool IsCastable() const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Status)
	float SkillDmg;

	UPROPERTY(EditDefaultsOnly, Category=Status)
	float SkillImpact;

	UPROPERTY(EditDefaultsOnly, Category=Status)
	FVector DmgBoxSize = FVector(60, 50, 44);

	UPROPERTY(EditDefaultsOnly, Category=Status)
	float BoxForwardBias = 100.f;

//	UPROPERTY(EditDefaultsOnly, Category = "Effects")
//	UParticleSystem* AttackImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* AttackImpactEffectNiagara;

private:
	bool bIsDashEffective = false;

	TSet<class AUSFightingCharacter*> AlreadyAttackedCharacters;

	UFUNCTION(NetMulticast, Unreliable)
	void CastAttackHitEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate);
};
