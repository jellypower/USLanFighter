// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Hatchet.h"

// Sets default values
AHatchet::AHatchet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHatchet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHatchet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

