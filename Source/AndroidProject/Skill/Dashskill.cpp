// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Dashskill.h"
#include "CharacterShare/EditorNames.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/USFightingCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UDashskill::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	FVector DmgBoxCenter = OwnerUSFighter->GetActorLocation() + OwnerUSFighter->GetActorForwardVector() * BoxForwardBias;
	
	if(bIsDashEffective)
	{
		TArray<FOverlapResult> OutOverlaps;

		
		GetWorld()->OverlapMultiByProfile(
			OutOverlaps,
			DmgBoxCenter,
			FQuat(1,0,0,0),
			CollisionName::Attack,
			FCollisionShape::MakeBox(DmgBoxSize));

		for(auto Result : OutOverlaps)
		{
			AUSFightingCharacter* Target =  Cast<AUSFightingCharacter>(Result.GetActor());
			if(IsValid(Target) && OwnerUSFighter != Target && !AlreadyAttackedCharacters.Contains(Target))
			{
				FVector AtkDir = Target->GetActorLocation() - GetOwner()->GetActorLocation();
				AtkDir.Normalize();

				constexpr float EFFECT_CAST_BIAS= 50.f;
				const FVector EffectLocation = Target->GetActorLocation() - AtkDir * EFFECT_CAST_BIAS;

				CastAttackHitEffect(EffectLocation, AtkDir.Rotation());
				
				Target->TakeDamage(SkillDmg, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
				Target->TakeImpact(SkillImpact, GetOwner()->GetInstigatorController(), GetOwner(),FVector2D(AtkDir));
				AlreadyAttackedCharacters.Add(Target);
			}
		}

#ifdef ENABLE_DRAW_DEBUG
	if(bIsDashEffective)
	{
		DrawDebugBox(
			GetWorld(),
			DmgBoxCenter,
			DmgBoxSize,
			FColor::Green,
			false
			);
		
	}
#endif
		
	}
}

void UDashskill::StartCast()
{
	SetCooldown();
}

void UDashskill::FinishCast()
{
	AlreadyAttackedCharacters.Empty();
	bIsDashEffective = false;
}

void UDashskill::InterruptCast()
{
	AlreadyAttackedCharacters.Empty();
	bIsDashEffective = false;
}

void UDashskill::TriggerEffect()
{
	bIsDashEffective = true;
}

bool UDashskill::IsCasting() const
{
	return bIsDashEffective;
}

bool UDashskill::IsCastable() const
{
	return CoolDownTimer <= 0;

}

void UDashskill::CastAttackHitEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackImpactEffectNiagara,
		spawnLocation, spawnRotate,
		FVector(1,1,1), true, true, ENCPoolMethod::AutoRelease);
}
