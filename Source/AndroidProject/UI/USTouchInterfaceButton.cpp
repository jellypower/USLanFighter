// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/USTouchInterfaceButton.h"
#include "Characters/USFightingCharacter.h"
#include "Skill/SkillComponentBase.h"

void UUSTouchInterfaceButton::BindPlayer(AUSFightingCharacter* InControllingCharacter)
{
	ControllingCharacter = InControllingCharacter;
	
	bool bExtracted =TryExtractSkillsFromWeapon(ControllingCharacter->GetCurEquippedWeapon());
		
	if(bExtracted)
				UE_LOG(LogTemp, Log, TEXT("%s: BindPlayer"),
        			GetOwningPlayer()->HasAuthority() ? TEXT("Server") : TEXT("Client"));
	
	ControllingCharacter->OnEquipWeapon.AddUniqueDynamic(this, &UUSTouchInterfaceButton::OnPlayerEquipWeapon);
}

void UUSTouchInterfaceButton::UnBindPlayer()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<3;j++)
		{
			OnButtonEvent[i][j].Unbind();
		}
	}
	
	ControllingCharacter = nullptr;
	Skill1 = nullptr;
	Skill2 = nullptr;

}


void UUSTouchInterfaceButton::OnPlayerEquipWeapon(AUSWeaponBase* Weapon)
{
	if(TryExtractSkillsFromWeapon(Weapon))
		UE_LOG(LogTemp, Log, TEXT("%s: OnPlayerEquipWeapon"),
			GetOwningPlayer()->HasAuthority() ? TEXT("Server") : TEXT("Client"));
}

float UUSTouchInterfaceButton::GetPlayerSkill1CooldownRatio() const
{
	if(IsValid(Skill1))
	{
		return FMath::Clamp(Skill1->GetCooldownTimer() / Skill1->GetCooldown(), 0, 1);
	}
	
	return 0;
}

float UUSTouchInterfaceButton::GetPlayerSkill2CooldownRatio() const
{
	if(IsValid(Skill2))
	{
		return FMath::Clamp(Skill2->GetCooldownTimer() / Skill2->GetCooldown(), 0, 1);
	}
	
	return 0;
}

bool UUSTouchInterfaceButton::TryExtractSkillsFromWeapon(const AUSWeaponBase* Weapon)
{
	if(IsValid(Weapon))
	{
		Skill1= Weapon->GetSkill(0);
		Skill2= Weapon->GetSkill(1);
		return true;
	}

	return false;
}
