// Copyright Epic Games, Inc. All Rights Reserved.

#include "USFightingCharacter.h"

#include "Algo/IndexOf.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Net/UnrealNetwork.h"

#include "UI/IngameCharacterInfo.h"


AUSFightingCharacter::AUSFightingCharacter()
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
	//NameTagWidget->SetIsReplicated(true);
	static ConstructorHelpers::FClassFinder<UUserWidget>
		NAME_TAG_ASSET(TEXT("/Game/UI/WB_InGamePlayerInfo.WB_InGamePlayerInfo_C"));


	if (NAME_TAG_ASSET.Succeeded())
	{
		NameTagWidget->SetWidgetClass(NAME_TAG_ASSET.Class);
		NameTagWidget->SetDrawSize(FVector2D(150, 50));
	}

	// Character Stat Init
	{
		BasicJumpIntensity = 600.f;
		BasicWalkSpeed = 600.f;
		MaxHP = 100.f;

	}

	ActionStateBitMask = 0;

}


void AUSFightingCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Client only
	if(IsLocallyControlled())
	{
		if(BufferedOrder.Type != FUSOrderType::None)
		{
			ExecuteOrder(BufferedOrder);
			BufferedOrder.Init();
		}

		PrintCharacterStateOnScreen();
		PrintCharacterStatOnScreen();
	}
	
	// Server only
	if(HasAuthority())
	{
		
	}

	
	// Update State
	{
		
		if(!GetMovementComponent()->IsFalling() && IsJumping())
			ActionStateBitMask = ActionStateBitMask & (~EUSPlayerActionState::Jump);
		
		if(GetMovementComponent()->IsMovingOnGround())
			ActionStateBitMask = ActionStateBitMask | EUSPlayerActionState::Move;
		else
			ActionStateBitMask = ActionStateBitMask & (~EUSPlayerActionState::Move);
	}


	
}


void AUSFightingCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();



	//Apply Character Stat
	{
		GetCharacterMovement()->JumpZVelocity = BasicJumpIntensity;
		GetCharacterMovement()->MaxAcceleration = TNumericLimits<float>::Max();
		GetCharacterMovement()->MaxWalkSpeed = BasicWalkSpeed;
	}
		if(IsLocallyControlled())
    		GEngine->AddOnScreenDebugMessage(13, 10.f, FColor::Red,
    			FString::Printf(
    			TEXT("Post init %f") ,	GetCharacterMovement()->MaxWalkSpeed));
}

void AUSFightingCharacter::BeginPlay()
{
	Super::BeginPlay();

	CastChecked<UIngameCharacterInfo>(NameTagWidget->GetWidget())->SetPlayer(this);
	CurHP = MaxHP;
}

void AUSFightingCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUSFightingCharacter, CurHP);
	DOREPLIFETIME(AUSFightingCharacter, MaxHP);
	DOREPLIFETIME(AUSFightingCharacter, CharacterName);
	DOREPLIFETIME(AUSFightingCharacter, ActionStateBitMask);
}


void AUSFightingCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	ActionStateBitMask = ActionStateBitMask | EUSPlayerActionState::Jump;
}

bool AUSFightingCharacter::IsMoveInputIgnored() const
{
	return Super::IsMoveInputIgnored() || GetCharacterMovement()->IsFalling();
}

float AUSFightingCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

float AUSFightingCharacter::TakeImpact()
{
	return 0;
}

void AUSFightingCharacter::ExecuteOrder(const FUSOrder& InOrder)
{
	switch (InOrder.Type)
	{
	case FUSOrderType::Move: // Character movement component automatically replicated to server
		AddMovementInput(InOrder.dir);
		break;
	case FUSOrderType::Jump:
		Jump();
		break;

		// TO-DO: send order request from client to server

		default:
			break;
	}

}

int8 AUSFightingCharacter::IsOrderAcceptable() const
{
	return (ActionStateBitMask & EUSPlayerActionStateValue::OrderUnAcceptableBitMask) == 0;
}

int8 AUSFightingCharacter::IsNextComboAtkAcceptable() const
{
	return (ActionStateBitMask & EUSPlayerActionState::Attack) != 0;
}

int8 AUSFightingCharacter::IsJumping() const
{
	return (ActionStateBitMask & EUSPlayerActionState::Jump) != 0;
}

int8 AUSFightingCharacter::IsMoving() const
{
	return (ActionStateBitMask & EUSPlayerActionState::Move) != 0;
}

void AUSFightingCharacter::OrderTo(FUSOrder InOrder)
{
	if (!IsOrderAcceptable())
	{
		BufferedOrder = InOrder;
		return;
	}

	ExecuteOrder(InOrder);
	
}
