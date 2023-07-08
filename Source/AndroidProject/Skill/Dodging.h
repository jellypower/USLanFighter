// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USAttackBlockable.h"
#include "Skill/SkillComponentBase.h"
#include "Dodging.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UDodging : public USkillComponentBase, public IUSAttackBlockable
{
	GENERATED_BODY()
public:
	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;

	/** This function is called by Montage's Animnotify when certain action is casted on certain timing*/
	virtual void TriggerEffect() override;
	
	virtual bool IsCastable() const override;
	virtual bool IsCasting() const override;

	virtual bool IsTakenImpactBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const override;
	virtual bool IsTakenDmgBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const override;
	virtual void OnBlocked
		(float TakenImpact, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) override;


protected:
	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float DodgingSlashDmg;

	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float DodgingSlashImpact;

private:
	bool bIsCasting = false;
};
