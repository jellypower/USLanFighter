// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "USCharacterAnim.generated.h"


/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UUSCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	
	void AnimateAttack(const uint8 InCurComboNum);
	void StopAnimateAttack();
	void AnimateImpacted(uint8 AnimateIdx);
	void AnimateBlown();

	void AnimateSkillCast(const uint8 InSkillIndex);

	uint8 IsAttackMotionPlaying() const;
	uint8 IsCastingMotionPlaying() const;
	uint8 IsAnyCastingMotionPlaying() const;
	void StopPlayingAnyMotion();
	uint8 GetTakeImpactAnimNum() const { return TakeImpactAnimNum; }
	
protected:
	UPROPERTY(BlueprintReadOnly)
	class AUSFightingCharacter* AnimatingCharacter;

	UPROPERTY(BlueprintReadOnly)
	class AUSWeaponBase* AnimatingCharacterWeapon;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> TakeImpactAnimMontage;

	UFUNCTION()
	void AnimNotify_OnNextActionCastable();
	
	UFUNCTION()
	void AnimNotify_OnEndOfEffectiveAttack();

	UFUNCTION()
	void AnimNotify_OnTriggerSkillEffect();

	uint8 TakeImpactAnimNum;

	UPROPERTY(BlueprintReadWrite)
	bool bImpactEnterTrigger = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool bBlownEnterTrigger = false;
	

	UFUNCTION()
	void RecoveryCharacterFromImpacted(UAnimMontage* Montage, bool bInterrupted);
	// called when hurt animation is over

	UFUNCTION()
	void ExitFromCastState(UAnimMontage* Montage, bool bInterrupted);

private:
	FOnMontageEnded OnImpactMontageEnded, OnAtkAnimateEnded;


};
