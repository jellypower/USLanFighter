// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponentBase.h"
#include "Whirlwind.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UWhirlwind : public USkillComponentBase
{
	GENERATED_BODY()
public:
	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;
	virtual void TriggerEffect() override;
	
	virtual bool IsCastable() const override;
	virtual bool IsCasting() const override;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	FVector DmgAreaSize;
	
	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float SwingDmg[3];

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float SwingImpact[3];
	
	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	class UNiagaraSystem* AttackImpactEffectNiagara;
	
private:
	uint8 SwingCount = 0;
	bool bIsCasting = false;

	UFUNCTION(NetMulticast, Unreliable)
	void CastAttackHitEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate);
};
