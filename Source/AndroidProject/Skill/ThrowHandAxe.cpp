// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/ThrowHandAxe.h"

#include "Weapons/USWeaponBase.h"
#include "BattleSystemBase/ProjectileBase.h"
#include "Characters/USFightingCharacter.h"
#include "CharacterShare/EditorNames.h"


void UThrowHandAxe::InitializeComponent()
{
	Super::InitializeComponent();

	FActorSpawnParameters params;

	if (IsValid(ProjectileClass) && GetOwner()->HasAuthority())
	{
		ProjectilePool = GetWorld()->SpawnActor<AProjectileBase>(*ProjectileClass, params);
		ProjectilePool->OnProjCollideToTarget.AddUObject(this, &UThrowHandAxe::ReturnProjToPoolOnCollision);
		ProjectilePool->OnProjCollideWithNothing.AddUObject(this, &UThrowHandAxe::ReturnProjToPoolOnNotCollision);
#if WITH_EDITOR
		ProjectilePool->SetIsTemporarilyHiddenInEditor(true);
#endif
	}
	UE_LOG(LogTemp, Log, TEXT("Init Axe Throwing"));
}

void UThrowHandAxe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(IsValid(ProjectilePool))
	{
		ProjectilePool->Destroy();
	}
}

void UThrowHandAxe::StartCast()
{
	UE_LOG(LogTemp, Log, TEXT("Skill Casting: Throw Hand Axe!!"));

	OwnerWeapon->StartAttack(SwingDmg, SwingImpact);
	SetCooldown();
	bIsCasting = true;
}

void UThrowHandAxe::FinishCast()
{
	OwnerWeapon->EndAttack();
	bIsCasting = false;
}

void UThrowHandAxe::InterruptCast()
{
	OwnerWeapon->EndAttack();
	bIsCasting = false;
}

void UThrowHandAxe::TriggerEffect()
{
	// Throw Axe
	ProjectilePool->SetOwner(OwnerUSFighter);

	constexpr float ProjectileForwardCoeff = 70.f;
	ProjectilePool->ActivateProjectile(
		OwnerUSFighter->GetActorLocation() + OwnerUSFighter->GetActorRotation().Vector() * ProjectileForwardCoeff,
		OwnerUSFighter->GetActorRotation().Vector(),
		ProjectileDmg, ProjSpeed);
	
}

bool UThrowHandAxe::IsCasting() const
{
	return bIsCasting;
}

bool UThrowHandAxe::IsCastable() const
{
	if(GetOwner()->HasAuthority())
		return CoolDownTimer <= 0 && IsValid(ProjectilePool);

	return CoolDownTimer <= 0 ;
	
}

void UThrowHandAxe::ReturnProjToPoolOnCollision(AProjectileBase* InProj, AUSFightingCharacter* Target)
{
	ProjectilePool->DeactivateProjectile();
}

void UThrowHandAxe::ReturnProjToPoolOnNotCollision(AProjectileBase* InProj)
{
	ProjectilePool->DeactivateProjectile();
}
