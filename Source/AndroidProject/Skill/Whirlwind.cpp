// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Whirlwind.h"
#include "CharacterShare/EditorNames.h"
#include "Characters/USFightingCharacter.h"

#include "Engine/DamageEvents.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapons/USWeaponBase.h"

void UWhirlwind::StartCast()
{
	SetCooldown();
	SwingCount = 0;
	bIsCasting = true;
	OwnerWeapon->StartSlashEffects();
}

void UWhirlwind::FinishCast()
{
	bIsCasting = false;
	OwnerWeapon->EndSlashEffect();
}

void UWhirlwind::InterruptCast()
{
	bIsCasting = false;
	OwnerWeapon->EndSlashEffect();
}

void UWhirlwind::TriggerEffect()
{
	TArray<FOverlapResult> OutOverlaps;

	FVector DmgAreaCenter = OwnerUSFighter->GetActorLocation();


	GetWorld()->OverlapMultiByProfile(
		OutOverlaps,
		DmgAreaCenter,
		FQuat(1, 0, 0, 0),
		CollisionName::Attack,
		FCollisionShape::MakeCapsule(DmgAreaSize));

	for (auto Result : OutOverlaps)
	{
		AUSFightingCharacter* Target = Cast<AUSFightingCharacter>(Result.GetActor());
		if (IsValid(Target) && OwnerUSFighter != Target)
		{
			FVector AtkDir = Target->GetActorLocation() - OwnerUSFighter->GetActorLocation();
			AtkDir.Normalize();

			constexpr float EFFECT_CAST_BIAS = 50.f;
			const FVector EffectLocation = Target->GetActorLocation() - AtkDir * EFFECT_CAST_BIAS;

			CastAttackHitEffect(EffectLocation, AtkDir.Rotation());

			Target->USTakeDamage(SwingDmg[SwingCount],  GetOwner()->GetInstigatorController(), OwnerUSFighter, this, FVector2D(AtkDir));
			Target->USTakeImpact(SwingImpact[SwingCount], GetOwner()->GetInstigatorController(), OwnerUSFighter, this,
			                   FVector2D(AtkDir));

		}
	}
	
	SwingCount++;


#ifdef ENABLE_DRAW_DEBUG

	DrawDebugCapsule(
		GetWorld(),
		DmgAreaCenter,
		DmgAreaSize.Z,
		DmgAreaSize.X,
		FQuat(0, 0, 0, 1),
		FColor::Green,
		false,
		1
	);

#endif
}

bool UWhirlwind::IsCastable() const
{
	return CoolDownTimer <= 0;
}


bool UWhirlwind::IsCasting() const
{
	return bIsCasting;
}

void UWhirlwind::CastAttackHitEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackImpactEffectNiagara,
	                                               spawnLocation, spawnRotate,
	                                               FVector(1, 1, 1), true, true, ENCPoolMethod::AutoRelease);
}
