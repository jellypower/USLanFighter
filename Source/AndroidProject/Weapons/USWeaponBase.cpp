// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/USWeaponBase.h"


#include "Characters/USCharacterAnim.h"
#include "CharacterShare/EditorNames.h"


// Sets default values
AUSWeaponBase::AUSWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Component"));
	SetRootComponent(WeaponMeshComp);
	WeaponMeshComp->SetRelativeTransform(FTransform());
	WeaponMeshComp->SetCollisionProfileName(CollisionName::Attack);

	WeaponDmgBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Damagebox Component"));
	WeaponDmgBoxComp->SetupAttachment(WeaponMeshComp);
	WeaponDmgBoxComp->SetCollisionProfileName(CollisionName::Attack);

}

void AUSWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AUSWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("wb:%s"),  *WeaponAnimBP->GetClass()->GetName());
	UE_LOG(LogTemp, Log, TEXT("wb:%s"),  *WeaponAnimBP->GetName());
}

// Called every frame
void AUSWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUSWeaponBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	
}

UClass* AUSWeaponBase::GetWeaponAnimBP() const
{
	return WeaponAnimBP;
}

