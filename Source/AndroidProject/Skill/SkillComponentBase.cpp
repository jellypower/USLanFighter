// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SkillComponentBase.h"
#include "Characters/USFightingCharacter.h"


#include "Net/UnrealNetwork.h"

USkillComponentBase::USkillComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
	
	
	bWantsInitializeComponent = true;
	
}

void USkillComponentBase::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CoolDownTimer > 0)
		CoolDownTimer-=DeltaTime;

}

void USkillComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillComponentBase, CoolDown);
	DOREPLIFETIME(USkillComponentBase, CoolDownTimer);
}

void USkillComponentBase::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Error, TEXT("Skill has bee destroyed!!"));
}

void USkillComponentBase::SetOwnerUSFighter(AUSFightingCharacter* InCharacter)
{

	if(!IsValid(InCharacter))
	{
		OwnerUSFighter = nullptr;
		OwnerWeapon = nullptr;
		return;
	}
	
	OwnerUSFighter = InCharacter;
	OwnerWeapon = InCharacter->GetCurEquippedWeapon();
}
