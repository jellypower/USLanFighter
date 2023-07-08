// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Dodging.h"

#include "Characters/USFightingCharacter.h"

void UDodging::StartCast()
{
	bIsCasting = true;
	SetCooldown();
	OwnerUSFighter->RecoveryFromImpacted();
	OwnerWeapon->StartAttack(DodgingSlashDmg, DodgingSlashImpact);
}

void UDodging::FinishCast()
{
	bIsCasting = false;
	OwnerWeapon->EndAttack();
}

void UDodging::InterruptCast()
{
	bIsCasting = false;
	OwnerWeapon->EndAttack();
}

void UDodging::TriggerEffect()
{
	OwnerWeapon->EndAttack();
}

bool UDodging::IsCastable() const
{
	return CoolDownTimer <= 0;
}

bool UDodging::IsCasting() const
{
	return bIsCasting;
}

bool UDodging::IsTakenImpactBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const
{
	return bIsCasting;
}

bool UDodging::IsTakenDmgBlockable
		(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir) const
{
	return bIsCasting;
}

void UDodging::OnBlocked
		(float TakenImpact, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir)
{
	// Do nothing
}
