// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponentBase.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANDROIDPROJECT_API USkillComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	

	USkillComponentBase();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	virtual void StartCast() PURE_VIRTUAL(USkillComponentBase::StartCast);
	virtual void FinishCast() PURE_VIRTUAL(USkillComponentBase::FinishCast);
	virtual void InterruptCast() PURE_VIRTUAL(USkillComponentBase::InterruptCast);

	/** This function is called by Montage's Animnotify when certain action is casted on certain timing*/
	virtual void TriggerEffect() PURE_VIRTUAL(USkillComponentBase::TriggerEffect);
	
	virtual bool IsCastable() const PURE_VIRTUAL(USkillComponentBase::IsCastable, return false; );
	virtual bool IsCasting() const PURE_VIRTUAL(USkillComponentBase::IsCasting, return false; );
	UAnimMontage* GetCastMotion() const { return CastMotion; }

	bool GetCastableOnImpacted(){ return bCastableOnImpacted; }
	bool GetCastableOnBlown(){ return bCastableOnBlown; }

	void SetOwnerUSFighter(class AUSFightingCharacter* InCharacter);



protected:

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	TObjectPtr<UAnimMontage> CastMotion;

	UPROPERTY(EditDefaultsOnly, Replicated, Category=SkillSetting)
	float CoolDown;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category=SkillSetting)
	float CoolDownTimer = 0;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	bool bCastableOnImpacted = false;

	UPROPERTY(EditDefaultsOnly, Category=SkillSetting)
	bool bCastableOnBlown = false;

	void SetCooldown(){ CoolDownTimer = CoolDown;}
	
	UPROPERTY()
	TObjectPtr<class AUSFightingCharacter> OwnerUSFighter;

	UPROPERTY()
	TObjectPtr<class AUSWeaponBase> OwnerWeapon;
		
};
