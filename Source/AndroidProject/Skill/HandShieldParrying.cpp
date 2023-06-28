// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/HandShieldParrying.h"

#include "Characters/USFightingCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UHandShieldParrying::StartCast()
{
	bIsCasting = true;
	SetCooldown();
}

void UHandShieldParrying::FinishCast()
{
	bIsCasting = false;
}

void UHandShieldParrying::InterruptCast()
{
	bIsCasting = false;
}

void UHandShieldParrying::TriggerEffect()
{
	// do nothing
}

bool UHandShieldParrying::IsCastable() const
{
	return CoolDownTimer <= 0 ;
}

bool UHandShieldParrying::IsCasting() const
{
	return bIsCasting;
}

bool UHandShieldParrying::IsTakenImpactBlockable
	(float ImpactAmount, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser, const FVector2D& AtkDir) const
{

	const FVector2D PlayerForward = FVector2D(OwnerUSFighter->GetActorForwardVector().GetSafeNormal());
	const float AngleSimilarity = FVector2D::DotProduct(PlayerForward, -AtkDir);
	const float AngleThreshold = UKismetMathLibrary::Cos(UKismetMathLibrary::DegreesToRadians(ImpactBlockableDegree));
	
	UE_LOG(LogTemp, Log, TEXT("Impact Block: %d && %d = %d"), bIsCasting, AngleSimilarity > AngleThreshold,
		bIsCasting && AngleSimilarity > AngleThreshold);
	
	return bIsCasting && AngleSimilarity > AngleThreshold;
}

bool UHandShieldParrying::IsTakenDmgBlockable
	(float ImpactAmount, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser, const FVector2D& AtkDir) const
{
	return IsTakenImpactBlockable(ImpactAmount, DmgCausedCharacter, DamageCauser, AtkDir);
}

void UHandShieldParrying::OnBlocked
	(float TakenImpact, const FVector2D& TakenImpactDir, AUSFightingCharacter* DmgCausedCharacter, AActor* DamageCauser)
{
	const FVector ImpactDir = (DmgCausedCharacter->GetActorLocation() - OwnerUSFighter->GetActorLocation())
		.GetSafeNormal();
	
	constexpr float EFFECT_CAST_BIAS = 50.f;
	const FVector EffectLocation = DmgCausedCharacter->GetActorLocation() - ImpactDir * EFFECT_CAST_BIAS;

	CastBlockEffect(EffectLocation, ImpactDir.Rotation());
	
	DmgCausedCharacter->USTakeImpact(TakenImpact, GetOwner()->GetInstigatorController(), OwnerUSFighter,
		FVector2D(ImpactDir));

	UE_LOG(LogTemp, Log, TEXT("%s took impact"), *DmgCausedCharacter->GetName());
}

void UHandShieldParrying::CastBlockEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleOnBlocked, spawnLocation, spawnRotate, true,
										 EPSCPoolMethod::AutoRelease);
}