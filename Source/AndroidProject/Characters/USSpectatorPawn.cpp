// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/USSpectatorPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Settings/USFightingGameState.h"

AUSSpectatorPawn::AUSSpectatorPawn()
{
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 4000.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bFindCameraComponentWhenViewTarget = true;
	bOnlyRelevantToOwner = true;
}

void AUSSpectatorPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USGameState = GetWorld()->GetGameState<AUSFightingGameState>();
}

void AUSSpectatorPawn::MoveToNextObservableActor()
{
	FindNextObservableActor_internal();
}

void AUSSpectatorPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(IsValid(ObservingActor))
	{
		SetActorLocation(ObservingActor->GetActorLocation());
	}
	else
	{
		FindNextObservableActor_internal();
		SetActorLocation(ObservingActor->GetActorLocation());
	}
}

void AUSSpectatorPawn::FindNextObservableActor_internal()
{
	CurrentObservingActorIdx++;
	if(USGameState->GetActiveCharacterCount() <= CurrentObservingActorIdx)
	{
		CurrentObservingActorIdx = -1; // Idx -1 means OverviewObservingActor
	}
	ObservingActor = USGameState->GetActorToView(CurrentObservingActorIdx);
}
