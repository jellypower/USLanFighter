// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"

#include "USTouchInterfaceButton.generated.h"




UENUM(BlueprintType)
enum class EUSButtonType : uint8
{
	AttackButton = 0,
	JumpButton = 1,
	Skill1Button,
	Skill2Button,
};

UENUM(BlueprintType)
enum class EUSBtnTouchTriggerEvent : uint8
{
	OnClicked = 0,
	OnPressed = 1,
	OnReleased
};


DECLARE_DELEGATE_OneParam(FButtonEventDelegate, const FInputActionValue& val);


/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UUSTouchInterfaceButton : public UUserWidget
{
	GENERATED_BODY()


private:

	UPROPERTY()
	const class AUSFightingCharacter* ControllingCharacter;

	UPROPERTY()
	const class USkillComponentBase* Skill1;

	UPROPERTY()
	const class USkillComponentBase* Skill2;
	
	FButtonEventDelegate OnButtonEvent[4][3];
	FInputActionValue boolValue = FInputActionValue(true);

public:
	template <class UserClass>
	void BindAction(EUSButtonType BtnType, EUSBtnTouchTriggerEvent BtnTriggerEvent, UserClass* obj,
					FButtonEventDelegate::TMethodPtr<UserClass> Func)
	{
		OnButtonEvent[static_cast<int8>(BtnType)][static_cast<int8>(BtnTriggerEvent)].BindUObject(obj, Func);
	}

	void BindPlayer(AUSFightingCharacter* InControllingCharacter);
	void UnBindPlayer();

	
	UFUNCTION(BlueprintCallable)
	void OnInputButton(EUSButtonType BtnType, EUSBtnTouchTriggerEvent BtnTriggerEvent)
	{
		OnButtonEvent[static_cast<int8>(BtnType)][static_cast<int8>(BtnTriggerEvent)].Execute(boolValue);
	}

protected:

	UFUNCTION()
	void OnPlayerEquipWeapon(class AUSWeaponBase* Weapon);

	UFUNCTION(BlueprintPure)
	float GetPlayerSkill1CooldownRatio() const;

	UFUNCTION(BlueprintPure)
	float GetPlayerSkill2CooldownRatio() const;

private:

	bool TryExtractSkillsFromWeapon(const class AUSWeaponBase* Weapon);
};