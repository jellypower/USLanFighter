// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/EarthBreak.h"
#include "CharacterShare/EditorNames.h"
#include "Characters/USFightingCharacter.h"


#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"

void UEarthBreak::StartCast()
{
	SetCooldown();
	bIsCasting = true;
}

void UEarthBreak::FinishCast()
{
	bIsCasting = false;
}

void UEarthBreak::InterruptCast()
{
	bIsCasting = false;
}

void UEarthBreak::TriggerEffect()
{

	TArray<FOverlapResult> OutOverlaps;

	constexpr float DMG_AREA_FRONT_BIAS = 100;
	const FVector DmgAreaCenter = OwnerUSFighter->GetActorLocation() +
			OwnerUSFighter->GetActorForwardVector() * DMG_AREA_FRONT_BIAS
			- OwnerUSFighter->GetActorUpVector() * OwnerUSFighter->GetSimpleCollisionHalfHeight();

	GetWorld()->OverlapMultiByProfile(
		OutOverlaps,
		DmgAreaCenter,
		FQuat(1, 0, 0, 0),
		CollisionName::Attack,
		FCollisionShape::MakeCapsule(DmgAreaSize));


	const FRotator AtkRot = OwnerUSFighter->GetActorRotation();
	
	CastAttackHitEffect(DmgAreaCenter, AtkRot, DmgAreaSize.X);

	for (auto Result : OutOverlaps)
	{
		AUSFightingCharacter* Target = Cast<AUSFightingCharacter>(Result.GetActor());
		if (IsValid(Target) && OwnerUSFighter != Target)
		{
			FVector AtkDir = Target->GetActorLocation() - DmgAreaCenter;
			AtkDir.Normalize();

			Target->USTakeImpact(BreakImpact, GetOwner()->GetInstigatorController(), OwnerUSFighter, this , FVector2D(FVector::UpVector));
			Target->USTakeDamage(BreakDmg,  GetOwner()->GetInstigatorController(), OwnerUSFighter, this, FVector2D(AtkDir));
		}
	}

#ifdef ENABLE_DRAW_DEBUG

	DrawDebugCapsule(
		GetWorld(),
		DmgAreaCenter,
		DmgAreaSize.Z,
		DmgAreaSize.X,
		FQuat(0, 0, 0, 1),
		FColor::Green,
		false,
		3
	);

#endif
}


bool UEarthBreak::IsCastable() const
{
	return CoolDownTimer <= 0;
}

bool UEarthBreak::IsCasting() const
{
	return bIsCasting;
}

void UEarthBreak::CastAttackHitEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate, float EffectWidth)
{
	constexpr float DEFAULT_MESH_WIDTH = 200;
	constexpr float EFFECT_MESH_MAX_SCALE = 1.5;
	constexpr float DEFAULT_EFFECT_WIDTH = DEFAULT_MESH_WIDTH * EFFECT_MESH_MAX_SCALE;

	const float Scale = EffectWidth/DEFAULT_EFFECT_WIDTH/2;
	UE_LOG(LogTemp, Log, TEXT("%f"), Scale);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackImpactEffectNiagara,
											   spawnLocation, spawnRotate,
											   FVector(Scale, Scale, Scale * 0.5), true, true, ENCPoolMethod::AutoRelease);

}
