// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectCharacter.h"

#include "BehaviorTree/Tasks/BTTask_MoveDirectlyToward.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"

#include "CharacterShare/EUSCharState.h"
#include "UI/IngameCharacterInfo.h"


AAndroidProjectCharacter::AAndroidProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1280.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 4000.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	NameTagWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTag"));
	NameTagWidget->SetupAttachment(GetMesh());
	NameTagWidget->SetRelativeLocation(FVector(0, 0, 180.f));
	NameTagWidget->SetWidgetSpace(EWidgetSpace::Screen);
	NameTagWidget->SetIsReplicated(true);
	static ConstructorHelpers::FClassFinder<UUserWidget>
		NAME_TAG_ASSET(TEXT("/Game/UI/PlayerInfoUI.PlayerInfoUI_C"));


	if (NAME_TAG_ASSET.Succeeded())
	{
		NameTagWidget->SetWidgetClass(NAME_TAG_ASSET.Class);
		NameTagWidget->SetDrawSize(FVector2D(150, 50));
	}

	// Character Stat Init
	{
		BasicJumpIntensity = 600.f;
		BasicWalkSpeed = 600.f;
		AirWalkAccel = 30.f;
	}
}

void AAndroidProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AAndroidProjectCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	//Apply Character Stat
	{
		GetCharacterMovement()->JumpZVelocity = BasicJumpIntensity;
		GetCharacterMovement()->MaxAcceleration = TNumericLimits<float>::Max();
		GetCharacterMovement()->MaxWalkSpeed = BasicWalkSpeed;
	}


}

void AAndroidProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	Cast<UIngameCharacterInfo>(NameTagWidget->GetWidget())->SetPlayer(this);

}


bool AAndroidProjectCharacter::IsMoveInputIgnored() const
{
	return Super::IsMoveInputIgnored() || GetCharacterMovement()->IsFalling();
}

void AAndroidProjectCharacter::OrderToJump()
{
	Jump();
}

void AAndroidProjectCharacter::OrderToMove(const FVector& dir)
{
	UE_LOG(LogTemp, Log, TEXT("%s: %f, %f %d"), *GetCharacterName(), dir.X, dir.Y, IsMoveInputIgnored());
	AddMovementInput(FVector(dir.Y, dir.X, 0));
}
