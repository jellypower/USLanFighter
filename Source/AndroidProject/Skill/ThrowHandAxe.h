// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponentBase.h"
#include "ThrowHandAxe.generated.h"

class AProjectileBase;
/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UThrowHandAxe : public USkillComponentBase
{
	GENERATED_BODY()
public:
	virtual void InitializeComponent() override;
	

public:
	virtual void StartCast() override;
	virtual void FinishCast() override;
	virtual void InterruptCast() override;
	virtual void TriggerEffect() override;
	virtual bool IsCasting() const override;
	virtual bool IsCastable() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float SwingDmg;

	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float SwingImpact;

	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float ProjectileDmg;

	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float ProjectileImpact;

	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	float ProjSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="SkillSetting/HandAxe")
	TSubclassOf<AProjectileBase> ProjectileClass;

private:

	UPROPERTY()
	AProjectileBase* ProjectilePool = nullptr;

	void ReturnProjToPoolOnCollision(AProjectileBase* InProj, AUSFightingCharacter* Target = nullptr);
	void ReturnProjToPoolOnNotCollision(AProjectileBase* InProj);
	
	bool bIsCasting = false;
};
