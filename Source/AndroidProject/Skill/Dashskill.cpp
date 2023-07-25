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

		//OwnerUSFighter->GetCharacterMovement()->add

		GetWorld()->OverlapMultiByProfile(
			OutOverlaps,
			DmgBoxCenter,
			OwnerUSFighter->GetActorRotation().Quaternion(),
			CollisionName::Attack,
			FCollisionShape::MakeBox(DmgBoxSize));

		for(auto Result : OutOverlaps)
		{
			AUSFightingCharacter* Target =  Cast<AUSFightingCharacter>(Result.GetActor());
			if(IsValid(Target) && OwnerUSFighter != Target && !AlreadyAttackedCharacters.Contains(Target))
			{
				FVector AtkDir = OwnerUSFighter->GetActorForwardVector();
				AtkDir.Normalize();

				constexpr float EFFECT_CAST_BIAS= 50.f;
				const FVector EffectLocation = Target->GetActorLocation() - AtkDir * EFFECT_CAST_BIAS;

				CastAttackHitEffect(EffectLocation, AtkDir.Rotation());
				
				Target->USTakeImpact(SkillImpact, GetOwner()->GetInstigatorController(), OwnerUSFighter, this, FVector2D(AtkDir));
				Target->USTakeDamage(SkillDmg, GetOwner()->GetInstigatorController(), OwnerUSFighter, this, FVector2D(AtkDir));
				AlreadyAttackedCharacters.Add(Target);

				UE_LOG(LogTemp, Log, TEXT("%s"), *GetOwner()->GetName());
			}
		}

#ifdef ENABLE_DRAW_DEBUG
	if(bIsDashEffective)
	{
		DrawDebugBox(
			GetWorld(),
			DmgBoxCenter,
			DmgBoxSize,
			OwnerUSFighter->GetActorRotation().Quaternion(),
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
	OwnerWeapon->StartSlashEffects();
	bIsDashEffective = true;

}

void UDashskill::FinishCast()
{
	AlreadyAttackedCharacters.Empty();
	OwnerWeapon->EndSlashEffect();
	bIsDashEffective = false;
}

void UDashskill::InterruptCast()
{
	AlreadyAttackedCharacters.Empty();
	bIsDashEffective = false;
}

void UDashskill::TriggerEffect()
{
	// Do nothing
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
