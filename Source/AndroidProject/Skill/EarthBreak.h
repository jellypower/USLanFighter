// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponentBase.h"
#include "EarthBreak.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UEarthBreak : public USkillComponentBase
{
	GENERATED_BODY()

	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;
	virtual void TriggerEffect() override;

	virtual bool IsCastable() const override;
	virtual bool IsCasting() const override;

protected:

	UPROPERTY(EditDefaultsOnly)
	FVector DmgAreaSize;

	UPROPERTY(EditDefaultsOnly)
	float DmgCenterFrontBias = 70;
	
	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	class UNiagaraSystem* AttackImpactEffectNiagara;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float BreakDmg;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float BreakImpact;
private:
	
	bool bIsCasting = false;

	UFUNCTION(NetMulticast, Reliable)
	void CastAttackHitEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate, float EffectWidth);
};
