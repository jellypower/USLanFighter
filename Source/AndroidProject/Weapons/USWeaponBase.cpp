// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/USWeaponBase.h"

// Sets default values
AUSWeaponBase::AUSWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Component"));
	SetRootComponent(WeaponMeshComp);
	
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

