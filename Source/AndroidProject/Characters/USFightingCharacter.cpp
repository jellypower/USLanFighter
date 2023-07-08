	// Copyright Epic Games, Inc. All Rights Reserved.

#include "USFightingCharacter.h"

#include "ClearReplacementShaders.h"
#include "USCharacterAnim.h"
#include "BattleSystemBase/ProjectileBase.h"
#include "Camera/CameraComponent.h"
#include "CharacterShare/EditorNames.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "UI/IngameCharacterInfo.h"
#include "Weapons/USWeaponBase.h"
#include "Skill/SkillComponentBase.h"
#include "Weapons/USDualWieldWeaponBase.h"
#include "Skill/USAttackBlockable.h"

AUSFightingCharacter::AUSFightingCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(21.f, 96.0f);

	DmgBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damage Box"));
	DmgBox->InitCapsuleSize(42, 88.f);

	DmgBox->SetCollisionObjectType(USCollisionChannel::DmgBoxChannel);
	DmgBox->SetupAttachment(GetRootComponent());

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

	SubWeaponSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Shield Socket"));
	SubWeaponSocket->SetupAttachment(GetMesh(), USSocketNames::Shield);
	SubWeaponSocket->SetRelativeLocation(FVector(0, 3.f, 0));
	SubWeaponSocket->SetRelativeRotation(FRotator(0, 0, -10));


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
	if (IsLocallyControlled())
	{
		if (BufferedOrder.IsMovementOrder() && IsOrderExecutableState()) // Process buffered order
		{
			SendOrderToServer(BufferedOrder);
			BufferedOrder.Init();
		}
		else if (BufferedOrder.IsCastOrder())
		{
			SendOrderToServer(BufferedOrder);
			BufferedOrder.Init();
		}
#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
		if (IsLocallyControlled())
		{
			PrintCharacterStateOnScreen();
			PrintCharacterStatOnScreen();

			GEngine->AddOnScreenDebugMessage(12, 10.f, FColor::Blue,
										 FString::Printf(TEXT(
										 	 "CharacterName: %s\n"
											 "CurCastingSkillName: %d, %s "
											 )
											 ,*GetName()
											 ,CurCastingSkill.IsValid()
											 ,CurCastingSkill.IsValid() ? *CurCastingSkill->GetName() : TEXT("None")));
		}
#endif
	}

	// Server only
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(13, 10.f, FColor::Blue,
		                                 FString::Printf(TEXT(
			                                 "X: %f, Y:%f, Z: %f"),
		                                                 GetCharacterMovement()->GetLastInputVector().X,
		                                                 GetCharacterMovement()->GetLastInputVector().Y,
		                                                 GetCharacterMovement()->GetLastInputVector().Z));

		SimulateStateUpdate(DeltaSeconds);


		if (ServerBufferedOrder.Type == FUSOrderType::None && !GetCharacterMovement()->GetCurrentAcceleration().
			IsNearlyZero())
		{
			ServerBufferedOrder.Type = FUSOrderType::Move;
		}

		if (ServerBufferedOrder.Type != FUSOrderType::None)
		{
			if(IsOrderExecutableState())
			{
				if (ServerBufferedOrder.Type != FUSOrderType::Attack && CharAnim->IsAnyMontagePlaying())
					StopAnimateAnyMotion();

				ExecuteOrderOnServer(ServerBufferedOrder);
				ServerBufferedOrder.Init();
			}
			else if(IsExecutableOrderInOrderNotExecutableState(ServerBufferedOrder))
			{
				ExecuteOrderOnServer(ServerBufferedOrder);
				ServerBufferedOrder.Init();
			}
		}
	}
}


void AUSFightingCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	{
		GetCharacterMovement()->JumpZVelocity = BasicJumpIntensity;
		GetCharacterMovement()->MaxAcceleration = TNumericLimits<float>::Max();
		GetCharacterMovement()->MaxWalkSpeed = BasicWalkSpeed;
	}
}

void AUSFightingCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurHP = MaxHP;
	}

	CastChecked<UIngameCharacterInfo>(NameTagWidget->GetWidget())->SetPlayer(this);
}

void AUSFightingCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if(IsValid(CurEquippedWeapon))
		CurEquippedWeapon->Destroy();
}


void AUSFightingCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUSFightingCharacter, CurHP);
	DOREPLIFETIME(AUSFightingCharacter, MaxHP);
	DOREPLIFETIME(AUSFightingCharacter, CharacterName);
	DOREPLIFETIME(AUSFightingCharacter, ActionStateBitMask);
	DOREPLIFETIME(AUSFightingCharacter, CurEquippedWeapon);
	DOREPLIFETIME(AUSFightingCharacter, CurComboIdx);
	DOREPLIFETIME(AUSFightingCharacter, PlayerIdentityColor);
}

void AUSFightingCharacter::OnRep_CurEquippedWeapon()
{
	EquipWeapon();
	UE_LOG(LogTemp, Log, TEXT("%s's cur equipped impact block num: %d"), *GetName(), AttackBlockingSkills.Num());
}

void AUSFightingCharacter::OnRep_PlayerIdentityColor()
{
	ApplyIdentityColor();
}

bool AUSFightingCharacter::IsExecutableOrderInOrderNotExecutableState(const FUSOrder& InOrder) const
{

	return 
	(IsImpacted() && InOrder.Type == FUSOrderType::Skill1 && CurEquippedWeapon->GetSkill(0)->GetCastableOnImpacted()) ||
	(IsImpacted() && InOrder.Type == FUSOrderType::Skill2 && CurEquippedWeapon->GetSkill(1)->GetCastableOnImpacted()) ||
	(IsBlown() && InOrder.Type == FUSOrderType::Skill1 && CurEquippedWeapon->GetSkill(0)->GetCastableOnBlown()) ||
	(IsBlown() && InOrder.Type == FUSOrderType::Skill2 && CurEquippedWeapon->GetSkill(1)->GetCastableOnBlown());
	
}

void AUSFightingCharacter::RecoveryFromImpacted()
{
	if(HasAuthority())
	{
		ActionStateBitMask = ActionStateBitMask & ~EUSPlayerActionState::Impacted;
	}
}

void AUSFightingCharacter::RecoveryFromBlown()
{
	if(HasAuthority())
	{
		ActionStateBitMask = ActionStateBitMask & ~EUSPlayerActionState::Blown;
	}
}

void AUSFightingCharacter::RecoveryFromCast()
{
	if(HasAuthority())
	{
		ActionStateBitMask = ActionStateBitMask & ~EUSPlayerActionState::Cast;
	}
}

void AUSFightingCharacter::HandWeaponToPlayer(UClass* InWeaponClass)
{
	FActorSpawnParameters params;
	params.Owner = this;
	params.Instigator = GetInstigator();

	CurEquippedWeapon = GetWorld()->SpawnActor<AUSWeaponBase>(InWeaponClass, params);
	CurEquippedWeapon->SetReplicates(true);

	EquipWeapon();
}

void AUSFightingCharacter::SetPlayerIdentityColor(FColor color)
{
	PlayerIdentityColor = color;
	ApplyIdentityColor();
}

void AUSFightingCharacter::EquipWeapon()
{
	check(CurEquippedWeapon != nullptr);

	CurEquippedWeapon->AttachToComponent(WeaponSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CurEquippedWeapon->SetOwner(this);
	CurEquippedWeapon->SetInstigator(this);

	GetCharacterMovement()->MaxWalkSpeed = CurEquippedWeapon->GetWalkSpeed();
	CurHP = CurEquippedWeapon->GetMaxHP() * (CurHP/MaxHP);
	MaxHP = CurEquippedWeapon->GetMaxHP();

	for (int i = 0; i < CurEquippedWeapon->GetTotalSkillNum(); i++)
	{
		if (IUSAttackBlockable* ImpactBlockingSkill = Cast<IUSAttackBlockable>(CurEquippedWeapon->GetSkill(i)))
		{
			AttackBlockingSkills.Add(ImpactBlockingSkill);
		}
	}

	if (const AUSDualWieldWeaponBase* DualWeapon = Cast<AUSDualWieldWeaponBase>(CurEquippedWeapon))
	{
		AActor* SubWeapon = DualWeapon->GetSubWeapon();

		SubWeapon->AttachToComponent(SubWeaponSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SubWeapon->SetOwner(this);
		UE_LOG(LogTemp, Log, TEXT("subweapon: %s"), *SubWeapon->GetName());
		SubWeapon->SetInstigator(this);
	}

	GetMesh()->SetAnimClass(CurEquippedWeapon->GetWeaponAnimBP());
	CharAnim = CastChecked<UUSCharacterAnim>(GetMesh()->GetAnimInstance());
}

void AUSFightingCharacter::ApplyIdentityColor()
{
	if(IdentityMaterial == nullptr)
	{
		IdentityMaterial = GetMesh()->CreateDynamicMaterialInstance(USConsts::IDENTITY_MATERIAL_IDX);
	}
	UE_LOG(LogTemp, Log, TEXT("Color: %d %d %d"), PlayerIdentityColor.R, PlayerIdentityColor.G, PlayerIdentityColor.B);
	IdentityMaterial->SetVectorParameterValue(USMaterialParamNames::Tint,
		FLinearColor(PlayerIdentityColor.R, PlayerIdentityColor.G, PlayerIdentityColor.B));
}


void AUSFightingCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (CharAnim->IsAnyCastingMotionPlaying()) CharAnim->StopPlayingAnyMotion();
	ActionStateBitMask = ActionStateBitMask | EUSPlayerActionState::Jump;
}

bool AUSFightingCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && IsOrderExecutableState();
}


bool AUSFightingCharacter::IsMoveInputIgnored() const
{
	return Super::IsMoveInputIgnored() || !IsOrderExecutableState();
}

float AUSFightingCharacter::USTakeDamage
(float DamageAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter ,UObject* DamageCauser, const FVector2D& AtkDir)
{
	if(HasAuthority())
	{
		for (auto AttackBlockingSkill : AttackBlockingSkills)
		{

			if (AttackBlockingSkill->IsTakenDmgBlockable
				(DamageAmount, EventInstigator, DmgCausedFighter, DamageCauser, AtkDir))
			{
				return 0;
			}
		}
		
		CurHP -= DamageAmount;

		return DamageAmount;
	}

	return DamageAmount;
}

float AUSFightingCharacter::USTakeImpact
	(float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir)
{
	if (HasAuthority())
	{
		for (auto ImpactBlockingSkill : AttackBlockingSkills)
		{

			if (DmgCausedFighter != nullptr && ImpactBlockingSkill->IsTakenImpactBlockable
				(ImpactAmount, EventInstigator, DmgCausedFighter, DamageCauser, AtkDir))
			{
				ImpactBlockingSkill->OnBlocked(ImpactAmount, EventInstigator, DmgCausedFighter, DamageCauser, AtkDir);
				return 0;
			}
		}

		if(ImpactAmount > USConsts::BLOWN_THRESHOLD && !IsBlown()) // If Blown possible
		{
			if (IsCasting())
				InterruptCastingOnServer();
			
			BlownTimer = DefaultBlownEndTime;
			
			SetActorRotation((-FVector(AtkDir, 0)).Rotation());

			LaunchCharacter(
				USConsts::LAUNCH_COEFF
				* ImpactAmount * FVector(AtkDir, 0.5), false, false);
			
			
			AnimateBlown();
			ActionStateBitMask = EUSPlayerActionState::Blown;

		}
		else if(ImpactAmount > 0) // If Impact possible
		{
			if (IsCasting())
				InterruptCastingOnServer();

			uint8 AnimateIdx = FMath::RandRange(0, CharAnim->GetTakeImpactAnimNum() - 1);
			AnimateImpacted(AnimateIdx);
			ImpactTimer = DefaultImpactTime;
			ActionStateBitMask = EUSPlayerActionState::Impacted;

		}
	}

	return ImpactAmount;
}


void AUSFightingCharacter::SendOrderToServer(const FUSOrder& InOrder)
{
	USkillComponentBase* SkillToCast;
	bool Castable;

	switch (InOrder.Type)
	{
	case FUSOrderType::Move: // Character movement component automatically replicated to server
		AddMovementInput(InOrder.dir);
		break;
	case FUSOrderType::Jump:
		AddMovementInput(InOrder.dir);
		Jump();
		SendOrderToServer_Internal_Reliable(InOrder); // Cancel the other cast orders on server 
		break;


	case FUSOrderType::Attack:
		SendOrderToServer_Internal_Reliable(InOrder);
		break;


	case FUSOrderType::Skill1:
		SkillToCast = CurEquippedWeapon->GetSkill(0);
		Castable = SkillToCast->IsCastable();
		if (Castable) SendOrderToServer_Internal_Reliable(InOrder);
		else OnCastNotCastableSkillOnClient.Broadcast(Castable, SkillToCast->GetName());
		break;

	case FUSOrderType::Skill2:
		SkillToCast = CurEquippedWeapon->GetSkill(1);
		Castable = SkillToCast->IsCastable();
		if (Castable) SendOrderToServer_Internal_Reliable(InOrder);
		else OnCastNotCastableSkillOnClient.Broadcast(Castable, SkillToCast->GetName());
		break;


	case FUSOrderType::Smash:
		SendOrderToServer_Internal_Reliable(InOrder);
		break;


	// TO-DO: send order request from client to server

	default:
		break;
	}
}

void AUSFightingCharacter::ExecuteOrderOnServer(const FUSOrder& InOrder)
{
	uint8 SkillIdx;
	USkillComponentBase* SkillToCast;

	if (IsOrderIgnoredOnServer(InOrder)) return;

	switch (InOrder.Type)
	{
	case FUSOrderType::Move:
	case FUSOrderType::Jump:
		// Character movement is replicated from client. So don't do anything on server
		break;

	case FUSOrderType::Attack:
		ExecuteAttackOnServer(FVector2D(InOrder.dir.X, InOrder.dir.Y));
		break;

	case FUSOrderType::Skill1:
		SkillIdx = 0;
		SkillToCast = CurEquippedWeapon->GetSkill(SkillIdx);
		if (SkillToCast->IsCastable()) ExecuteSkillOnServer(SkillIdx, FVector2D(InOrder.dir));
		break;
	case FUSOrderType::Skill2:
		SkillIdx = 1;
		SkillToCast = CurEquippedWeapon->GetSkill(SkillIdx);
		if (SkillToCast->IsCastable()) ExecuteSkillOnServer(SkillIdx, FVector2D(InOrder.dir));
		break;
	}
}

void AUSFightingCharacter::SimulateStateUpdate(float DeltaTime)
{
	if (!GetCharacterMovement()->IsFalling() && IsJumping() || !IsJumping())
		ActionStateBitMask = ActionStateBitMask & (~EUSPlayerActionState::Jump);

	if (GetCharacterMovement()->IsMovingOnGround())
		ActionStateBitMask = ActionStateBitMask | EUSPlayerActionState::Move;
	else
		ActionStateBitMask = ActionStateBitMask & (~EUSPlayerActionState::Move);

	if(ImpactTimer > 0)
		ImpactTimer -= DeltaTime;
		
	if(BlownTimer > 0 && !GetCharacterMovement()->IsFalling())
		BlownTimer -= DeltaTime;

	if(IsImpacted() && ImpactTimer <= 0)
	{
		RecoveryFromImpacted();
	}
	if(IsBlown() && BlownTimer <= 0)
	{
		RecoveryFromBlown();
	}
}


void AUSFightingCharacter::OrderTo(FUSOrder InOrder)
{
	if (IsInputIgnoredToImproveSenseOfOperation(InOrder))
	{
		return;
	}

	if (IsOrderExecutableState() || IsExecutableOrderInOrderNotExecutableState(InOrder))
	{
		SendOrderToServer(InOrder);
	}
	else
	{
		BufferedOrder = InOrder;
	}

}


void AUSFightingCharacter::SendOrderToServer_Internal_Reliable_Implementation(FUSOrder InOrder)
{
	ServerBufferedOrder = InOrder;
}


void AUSFightingCharacter::StartCastProcessOnServer()
{
	ActionStateBitMask = ActionStateBitMask | EUSPlayerActionState::Cast;
}

void AUSFightingCharacter::ExecuteAttackOnServer(const FVector2D& Dir)
{
	StartCastProcessOnServer();

	FVector Dir3D = FVector(Dir, 0);
	if (!Dir3D.IsNearlyZero())
		SetActorRotation(Dir3D.Rotation());
	
	if (CharAnim->IsAttackMotionPlaying() && CurComboIdx < CurEquippedWeapon->GetComboMaxNum() - 1)
		CurComboIdx++;
	else
		CurComboIdx = 0;

	checkf(CurComboIdx < CurEquippedWeapon->GetComboMaxNum(),
		   TEXT("Cur combo idx must be lower than MaxCombo"));
	
	CurEquippedWeapon->StartAttack(
	CurEquippedWeapon->GetWeaponDmg(CurComboIdx),
	CurEquippedWeapon->GetWeaponImpact(CurComboIdx));

	AnimateAttack(CurComboIdx);
}

void AUSFightingCharacter::ExecuteSkillOnServer(uint8 skillIdx, const FVector2D& Dir)
{
#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	if (CurEquippedWeapon->GetSkill(skillIdx) == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill is not allocated"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ExecuteSkill"));

#endif
	StartCastProcessOnServer();

	CurCastingSkill = CurEquippedWeapon->GetSkill(skillIdx);
	CurCastingSkill->StartCast();

	FVector Dir3D = FVector(Dir, 0);
	if (!Dir.IsNearlyZero())
		SetActorRotation(Dir3D.Rotation());
	AnimateSkillCasting(skillIdx);
}


void AUSFightingCharacter::FinishCasting()
{
	if (HasAuthority())
	{
		EndCastProcessOnServer(false);
	}
}

void AUSFightingCharacter::TriggerSkillEffect()
{
	if (HasAuthority())
	{
		checkf(CurCastingSkill.IsValid(), TEXT("To trigger skill, Cur Casting Skill must be valid"));
		
		CurCastingSkill->TriggerEffect();
	}
}

void AUSFightingCharacter::InterruptCastingOnServer()
{
	StopAnimateAnyMotion();
	EndCastProcessOnServer(true);
}


void AUSFightingCharacter::EndCastProcessOnServer(bool bIsInterrupt)
{
	ActionStateBitMask = ActionStateBitMask & ~EUSPlayerActionState::Cast;
	CurEquippedWeapon->EndAttack();

	if (CurCastingSkill.IsValid())
	{
		if (bIsInterrupt) CurCastingSkill->InterruptCast();
		else CurCastingSkill->FinishCast();
		UE_LOG(LogTemp, Log, TEXT("Interrupted: %d"), (int)bIsInterrupt);
		CurCastingSkill = nullptr;
	}
}

void AUSFightingCharacter::AnimateAttack_Implementation(const uint8 InCurComboNum)
{
	if (!IsValid(GetCurEquippedWeapon())) return;
	CharAnim->AnimateAttack(InCurComboNum);
}

void AUSFightingCharacter::StopAnimateAttack_Implementation()
{
	if (!IsValid(GetCurEquippedWeapon())) return;
	CharAnim->StopAnimateAttack();
}

void AUSFightingCharacter::StopAnimateAnyMotion_Implementation()
{
	if (!IsValid(GetCurEquippedWeapon())) return;
	CharAnim->StopPlayingAnyMotion();
}

void AUSFightingCharacter::AnimateSkillCasting_Implementation(uint8 skillIdx)
{
	if (!IsValid(GetCurEquippedWeapon())) return;
	CharAnim->AnimateSkillCast(skillIdx);
}

void AUSFightingCharacter::AnimateImpacted_Implementation(uint8 AnimateIdx)
{
	if (!IsValid(GetCurEquippedWeapon())) return;
	CharAnim->AnimateImpacted(AnimateIdx);
}

void AUSFightingCharacter::AnimateBlown_Implementation()
{
	if (!IsValid(GetCurEquippedWeapon())) return;
		CharAnim->AnimateBlown();
}
