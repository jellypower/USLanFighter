// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponentBase.h"

#include "USAttackBlockable.h"

#include "HandShieldParrying.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UHandShieldParrying : public USkillComponentBase, public IUSAttackBlockable
{
	GENERATED_BODY()
public:
	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;
	virtual void TriggerEffect() override;
	
	virtual bool IsCastable() const override;
	virtual bool IsCasting() const override;


	virtual bool IsTakenImpactBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const override;
	virtual bool IsTakenDmgBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const override;
	virtual void OnBlocked
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float ImpactBlockableDegree = 45;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	float InflicatingImpactAmountOnBlocked = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	UParticleSystem* ParticleOnBlocked;
	
private:
	bool bIsCasting = false;

	UFUNCTION(NetMulticast, Unreliable)
	void CastBlockEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate);
	
};

