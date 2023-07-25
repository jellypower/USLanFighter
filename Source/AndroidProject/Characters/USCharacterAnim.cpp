// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/USCharacterAnim.h"

#include "CharacterShare/EditorNames.h"
#include "USFightingCharacter.h"
#include "Weapons/USWeaponBase.h"
#include "Skill/SkillComponentBase.h"

void UUSCharacterAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	

	AnimatingCharacter = Cast<AUSFightingCharacter>(GetOwningActor());
	if (AnimatingCharacter)
	{
		AnimatingCharacterWeapon = AnimatingCharacter->GetCurEquippedWeapon();
	}

	TakeImpactAnimNum = TakeImpactAnimMontage.Num();
	
	//OnAtkAnimateEnded.BindUObject(this, &UUSCharacterAnim::ExitFromCastStateOnBasicAtk);
	
}


void UUSCharacterAnim::AnimateAttack(const uint8 InCurComboNum)
{
	
	if (!Montage_IsPlaying(AnimatingCharacterWeapon->GetPlayerAttackAnim()))
		Montage_Play(AnimatingCharacterWeapon->GetPlayerAttackAnim());
	Montage_JumpToSection(MontageSectionNames::Combo[InCurComboNum]);

	//Montage_SetEndDelegate(OnAtkAnimateEnded, AnimatingCharacterWeapon->GetPlayerAttackAnim());
	
}

void UUSCharacterAnim::StopAnimateAttack()
{
	
	if (Montage_IsPlaying(AnimatingCharacterWeapon->GetPlayerAttackAnim()))
	{
		constexpr float BLEND_TIME = 0.1;
		Montage_Stop(BLEND_TIME);
	}
}

void UUSCharacterAnim::AnimateImpacted(uint8 AnimateIdx)
{
	Montage_Play(TakeImpactAnimMontage[AnimateIdx]);
	//if(AnimatingCharacter->HasAuthority())
//		Montage_SetEndDelegate(OnImpactMontageEnded, TakeImpactAnimMontage[AnimateIdx]);

}

void UUSCharacterAnim::AnimateBlown()
{
	bBlownEnterTrigger = true;
}


void UUSCharacterAnim::AnimateSkillCast(const uint8 InSkillIndex)
{
	Montage_Play(AnimatingCharacterWeapon->GetSkill(InSkillIndex)->GetCastMotion());
}


uint8 UUSCharacterAnim::IsAttackMotionPlaying() const
{
	return Montage_IsPlaying(AnimatingCharacterWeapon->GetPlayerAttackAnim());
}

uint8 UUSCharacterAnim::IsCastingMotionPlaying() const
{

	for(int i=0;i<AnimatingCharacterWeapon->GetTotalSkillNum();i++)
	{
		if(Montage_IsPlaying(AnimatingCharacterWeapon->GetSkill(i)->GetCastMotion()))
			return true;
	}
		

	return false;

}

uint8 UUSCharacterAnim::IsAnyCastingMotionPlaying() const
{
	return IsAttackMotionPlaying() || IsCastingMotionPlaying();
}

void UUSCharacterAnim::StopPlayingAnyMotion()
{
	constexpr float BLEND_TIME = 0.1;
	StopAllMontages(BLEND_TIME);
}



void UUSCharacterAnim::AnimNotify_OnNextActionCastable()
{
	AnimatingCharacter->FinishCasting();
}

void UUSCharacterAnim::AnimNotify_OnEndOfEffectiveAttack()
{
	AnimatingCharacterWeapon->EndAttack();
}

void UUSCharacterAnim::AnimNotify_OnTriggerSkillEffect()
{
	AnimatingCharacter->TriggerSkillEffect();
}


bool UUSCharacterAnim::IsCharacterBlown() const
{
	return AnimatingCharacter->IsBlown();
}

bool UUSCharacterAnim::IsCharacterDeath() const
{
	return AnimatingCharacter->IsDied();
}


void UUSCharacterAnim::ExitFromCastStateOnBasicAtk(UAnimMontage* Montage, bool bInterrupted)
{
	AnimatingCharacter->RecoveryFromCast();
	AnimNotify_OnEndOfEffectiveAttack();
}
