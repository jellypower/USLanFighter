// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSystemBase/ProjectileBase.h"

#include "Algo/Rotate.h"
#include "Characters/USFightingCharacter.h"
#include "CharacterShare/EditorNames.h"

#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh Component"));
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetSimulatePhysics(false);
	ProjectileMesh->SetCollisionProfileName(CollisionName::NoCollision);
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetIsReplicated(true);

	ProjectileDmgBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Damagebox Component"));
	ProjectileDmgBox->SetupAttachment(ProjectileMesh);
	ProjectileDmgBox->SetCollisionProfileName(CollisionName::Attack);
	ProjectileDmgBox->SetGenerateOverlapEvents(true);
	ProjectileDmgBox->SetIsReplicated(false);

	ProjectileTrailComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle Component"));
	ProjectileTrailComp->SetupAttachment(GetRootComponent());
	ProjectileTrailComp->SetIsReplicated(true);

	ProjMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjMovementComp->SetUpdatedComponent(GetRootComponent());
	ProjMovementComp->ProjectileGravityScale = 0;
	ProjMovementComp->SetIsReplicated(true);
	ProjMovementComp->bAutoActivate = true;

	SetActorRotation(FRotator(0,0,90));

	
	DeactivateProjectile_Internal();
}


void AProjectileBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if(LifeTimer <= 0)
	{
		OnProjCollideWithNothing.Broadcast(this);
	}
	else
	{
		LifeTimer -= DeltaSeconds;
		AddActorLocalRotation(FRotator(0,0,RotateSpeed * DeltaSeconds));
	}
}

void AProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AUSFightingCharacter* OtherUSChar = Cast<AUSFightingCharacter>(OtherActor);

	if (OtherUSChar != OwnerFighter)
	{
		OnProjCollision(OtherUSChar);
		DeactivateProjectile();
	}
}

void AProjectileBase::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	OwnerFighter = NewOwner != nullptr ? CastChecked<AUSFightingCharacter>(NewOwner) : nullptr;
}


void AProjectileBase::DeactivateProjectile()
{
	DeactivateProjectile_Internal();
}

void AProjectileBase::ActivateProjectile(FVector SpawnPos, FVector CastDir, float Dmg, float Speed)
{
	ProjectileDmg = Dmg;
	SetActorLocation(SpawnPos);
	SetActorRotation(FRotator(0, 0, 90));
	
	AActor::SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	ReplicateProjectileSpeed(CastDir, Speed);
	StartSlashEffect();

	LifeTimer = LifeTime;
	bIsProjActive = true;
}

void AProjectileBase::OnProjCollision(AUSFightingCharacter* Target)
{
	FDamageEvent DmgEvent;

	FVector AtkDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector2D AtkDir2D = FVector2D(AtkDir.X, AtkDir.Y);

	constexpr float EFFECT_CAST_COEFF = 50.f;
	FVector CollisionPos = Target->GetActorLocation() - AtkDir * EFFECT_CAST_COEFF;

	Target->USTakeDamage(ProjectileDmg, AtkDir2D, GetInstigatorController(), OwnerFighter.Get());
	Target->USTakeImpact(ProjectileDmg, GetInstigatorController(), OwnerFighter.Get(), AtkDir2D);

	CastAttackHitEffect(CollisionPos, AtkDir.Rotation());

	OnProjCollideToTarget.Broadcast(this, Target);
}

void AProjectileBase::CastAttackHitEffect_Implementation(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, AttackImpactEffect, spawnLocation, spawnRotate, true,
	                                         EPSCPoolMethod::AutoRelease);
}

void AProjectileBase::ReplicateProjectileSpeed_Implementation(FVector CastDir, float Speed)
{
	ProjMovementComp->InitialSpeed = Speed;
	ProjMovementComp->MaxSpeed = Speed;
	ProjMovementComp->Velocity = CastDir * Speed;
}

void AProjectileBase::StartSlashEffect_Implementation()
{
	ProjectileTrailComp->BeginTrails(USSocketNames::Weapon_Socket_slash_start, USSocketNames::Weapon_Socket_slash_end,
							   ETrailWidthMode_FromCentre, 10);
}

void AProjectileBase::EndSlashEffect_Implementation()
{
	ProjectileTrailComp->EndTrails();

}

void AProjectileBase::DeactivateProjectile_Internal()
{
	AActor::SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);


	ReplicateProjectileSpeed(FVector::Zero(), 0);
	EndSlashEffect();

	bIsProjActive = false;
}
