// Copyright Epic Games, Inc. All Rights Reserved.

#include "USFightingCharacter.h"

#include "USCharacterAnim.h"
#include "Algo/IndexOf.h"
#include "Camera/CameraComponent.h"
#include "CharacterShare/EditorNames.h"
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
#include "Weapons/USWeaponBase.h"


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

	WeaponSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Socket"));
	WeaponSocket->SetupAttachment(GetMesh(), USSocketNames::Weapon);

	ShieldSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Shield Socket"));
	ShieldSocket->SetupAttachment(GetMesh(), USSocketNames::Shield);
	ShieldSocket->SetRelativeLocation(FVector(0, 3.f, 0));
	ShieldSocket->SetRelativeRotation(FRotator(0,0,-10));

	

	// Character Stat Init
	BasicJumpIntensity = 600.f;
	BasicWalkSpeed = 600.f;
	MaxHP = 100.f;

	// Character State Init
	ActionStateBitMask = 0;


}


void AUSFightingCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Client only
	if(IsLocallyControlled())
	{
		if(BufferedOrder.Type != FUSOrderType::None && IsOrderAcceptable())
		{
			ExecuteOrder(BufferedOrder);
			BufferedOrder.Init();
		}

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
		PrintCharacterStateOnScreen();
		PrintCharacterStatOnScreen();
#endif
		
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
	
}

void AUSFightingCharacter::BeginPlay()
{
	Super::BeginPlay();

	EquipWeapon(WeaponClass);
	
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


void AUSFightingCharacter::EquipWeapon(UClass* InWeaponClass)
{
	const FVector spawnLocation = WeaponSocket->GetComponentLocation();
	const FRotator spawnRotate = WeaponSocket->GetComponentRotation();
	
	FActorSpawnParameters params;
	params.Owner = this;
	params.Instigator = GetInstigator();
	
	CurEquippedWeapon = GetWorld()->SpawnActor<AUSWeaponBase>(InWeaponClass, params);

	check(CurEquippedWeapon != nullptr);

	FAttachmentTransformRules rule(EAttachmentRule::SnapToTarget, true);

	CurEquippedWeapon->AttachToComponent(WeaponSocket, rule);

	GetMesh()->SetAnimClass(CurEquippedWeapon->GetWeaponAnimBP());
	//UE_LOG(LogTemp, Log, TEXT("%s"), *GetMesh()->GetAnimInstance()->GetName());
	UE_LOG(LogTemp, Log, TEXT("%s"), *CurEquippedWeapon->GetWeaponAnimBP()->GetName());
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
	case FUSOrderType::Attack:
		Attack();
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

void AUSFightingCharacter::Attack_Implementation()
{
	
	
}
