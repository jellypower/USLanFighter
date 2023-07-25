// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MatchingSystemManagingWidget.h"
#include "MainMenuWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API UMainMenuWidgetBase : public UMatchingSystemManagingWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
protected:

	uint8 CurWeaponIdx = 0;

	UPROPERTY(BlueprintReadOnly)
	FString WeaponNameToEquip;

	UPROPERTY(EditDefaultsOnly)
	class UWeaponList* AvailableWeaponList;
	
	UFUNCTION(BlueprintCallable)
	void SetNextWeaponNameToEquip();
	
	UFUNCTION(BlueprintCallable)
	void SetPrevWeaponNameToEquip();
	


};
