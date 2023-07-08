// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"


class UProjectileMovementComponent;
class AUSWeaponBase;
class AUSFightingCharacter;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProjCollideToTarget, AProjectileBase* Proj, class AUSFightingCharacter* Target);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjCollideWithNothing, AProjectileBase* Proj);

UCLASS(Blueprintable)
class ANDROIDPROJECT_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileBase();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


public:

	virtual void SetOwner(AActor* NewOwner) override;
	AUSFightingCharacter* GetOwnerUsFighter() const;

	virtual void DeactivateProjectile();
	virtual void ActivateProjectile(FVector SpawnPos, FVector CastDir, float Dmg, float Speed);

	virtual void OnProjCollision(class AUSFightingCharacter* Target);

	FOnProjCollideToTarget OnProjCollideToTarget;
	FOnProjCollideWithNothing OnProjCollideWithNothing;
	

protected:
	UPROPERTY(VisibleAnywhere, Category="Projectile Base Component")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category="Projectile Base Component")
	class UBoxComponent* ProjectileDmgBox;

	UPROPERTY(VisibleAnywhere, Category="Projectile Base Component")
	UParticleSystemComponent* ProjectileTrailComp;

	UPROPERTY(VisibleAnywhere, Category="Projectile Base Component")
	UProjectileMovementComponent* ProjMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Base Component")
	UParticleSystem* AttackImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Base Component")
	float RotateSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Base Component")
	float LifeTime;

	float LifeTimer;
	float ProjectileDmg;
	float ProjectileImpact;
	FVector ThrowingDir;
private:
	bool bIsProjActive;

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void CastAttackHitEffect(FVector_NetQuantize100 spawnLocation, FRotator spawnRotate);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateProjectileSpeed(FVector CastDir, float Speed);

	UFUNCTION(NetMulticast, Unreliable)
	void StartSlashEffect();

	UFUNCTION(NetMulticast, Reliable)
	void EndSlashEffect();
	
	void DeactivateProjectile_Internal();

	TWeakObjectPtr<AUSFightingCharacter> OwnerFighter;
};
