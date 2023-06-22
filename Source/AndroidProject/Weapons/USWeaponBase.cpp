// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/USWeaponBase.h"

#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"


#include "Characters/USCharacterAnim.h"
#include "Characters/USFightingCharacter.h"
#include "CharacterShare/EditorNames.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Skill/SkillComponentBase.h"

// Sets default values
AUSWeaponBase::AUSWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Component"));
	SetRootComponent(WeaponMeshComp);
	WeaponMeshComp->SetRelativeTransform(FTransform());
	WeaponMeshComp->SetCollisionProfileName(CollisionName::Attack);
	WeaponMeshComp->SetGenerateOverlapEvents(false);

	WeaponDmgBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Damagebox Component"));
	WeaponDmgBoxComp->SetupAttachment(WeaponMeshComp);
	WeaponDmgBoxComp->SetCollisionProfileName(CollisionName::Attack);
	WeaponDmgBoxComp->SetGenerateOverlapEvents(true);
	WeaponDmgBoxComp->SetIsReplicated(false);

	TrailParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle Component"));
	TrailParticleComp->SetupAttachment(WeaponMeshComp);
	TrailParticleComp->SetIsReplicated(true);


	AlreadyAttackedCharacters.Empty();
}

void AUSWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsValid(AttackMotionMontage))
		ComboMaxNum = AttackMotionMontage->GetNumSections();
	
	TArray<USkillComponentBase*> Comps;
	GetComponents<USkillComponentBase>(Comps);

	if(Comps.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s No skill on weapon"), *GetName());
	}
	
	for (auto Skill : Comps)
	{
		UE_LOG(LogTemp, Log, TEXT("%s skill equipped"), *Skill->GetName());
		Skills.Add(Skill);
	}
}


// Called when the game starts or when spawned
void AUSWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AUSWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUSWeaponBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AUSFightingCharacter* OtherUsFightingCharacter = CastChecked<AUSFightingCharacter>(OtherActor);

	if (OtherActor != GetOwner() && !AlreadyAttackedCharacters.Contains(OtherUsFightingCharacter))
	{
		Super::NotifyActorBeginOverlap(OtherActor);

		AlreadyAttackedCharacters.Add(OtherUsFightingCharacter);

		if (bIsWeaponDmgEffective || bIsWeaponImpactEffective)
		{
			//struct FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rot, FName ProfileName, const struct FCollisionShape& CollisionShape, const struct FCollisionQueryParams& Params


			constexpr float EFFECT_CAST_BIAS = 50;

			const FVector atkDir = (WeaponDmgBoxComp->GetComponentLocation() - OtherActor->GetActorLocation()).
				GetSafeNormal();
			const FVector spawnLocation = OtherActor->GetActorLocation()
				+ atkDir * EFFECT_CAST_BIAS;

			const FRotator spawnRotate = atkDir.Rotation();


			CastAttackHitEffect(spawnLocation, spawnRotate);
		}

		if (bIsWeaponDmgEffective)
		{
			FDamageEvent DmgEvent;
			OtherUsFightingCharacter->TakeDamage(curWeaponDmg, DmgEvent, GetOwner()->GetInstigatorController(), this);
		}

		if (bIsWeaponImpactEffective)
		{
			OtherUsFightingCharacter->TakeImpact(curWeaponImpact, GetOwner()->GetInstigatorController(), this
			                                     , FVector2d(
				                                     OtherActor->GetActorLocation() - Owner->GetActorLocation()));
		}
	}
}

void AUSWeaponBase::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

		OwnerAUSCharacter =
			NewOwner != nullptr ? CastChecked<AUSFightingCharacter>(NewOwner) : nullptr;

	
	for(auto skill : Skills)
	{
		skill->SetOwnerUSFighter(OwnerAUSCharacter);
		skill->SetComponentTickEnabled(true);
	}
}

void AUSWeaponBase::StartSlashEffects_Implementation()
{
	TrailParticleComp->BeginTrails(USSocketNames::Weapon_Socket_slash_start, USSocketNames::Weapon_Socket_slash_end,
	                               ETrailWidthMode_FromCentre, 0.5);
}

void AUSWeaponBase::EndSlashEffect_Implementation()
{
	TrailParticleComp->EndTrails();
}

void AUSWeaponBase::CastAttackHitEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, AttackImpactEffect, spawnLocation, spawnRotate, true,
	                                         EPSCPoolMethod::AutoRelease);
}

USkillComponentBase* AUSWeaponBase::GetSkill(uint8 SkillIdx) const
{
#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	if(!Skills.IsValidIndex(SkillIdx)) return nullptr;
#endif
	
	return Skills[SkillIdx];
}

UClass* AUSWeaponBase::GetWeaponAnimBP() const
{
	return WeaponAnimBP;
}

TObjectPtr<AUSFightingCharacter> AUSWeaponBase::GetOwnerAUSCharacter() const
{
	return OwnerAUSCharacter;
}

void AUSWeaponBase::StartAttack(float InDmg, float InImpact)
{
	StartGivingDmg(InDmg);
	StartGivingImpact(InImpact);
	AlreadyAttackedCharacters.Empty();

	StartSlashEffects();
}

void AUSWeaponBase::StartGivingDmg(float InDmg)
{
	bIsWeaponDmgEffective = true;
	curWeaponDmg = InDmg;
}

void AUSWeaponBase::StartGivingImpact(float InImpact)
{
	bIsWeaponImpactEffective = true;
	curWeaponImpact = InImpact;
}

void AUSWeaponBase::EndAttack()
{
	bIsWeaponDmgEffective = false;
	bIsWeaponImpactEffective = false;
	curWeaponDmg = 0;
	curWeaponImpact = 0;

	EndSlashEffect();
}
