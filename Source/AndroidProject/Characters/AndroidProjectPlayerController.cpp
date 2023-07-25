// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InputDataAsset/InputActionDataAsset.h"
#include "GameFramework/TouchInterface.h"


#include "USFightingCharacter.h"
#include "USSpectatorPawn.h"
#include "UI/USTouchInterfaceButton.h"
#include "UI/USPlayerCharacterHUD.h"
#include "Settings/USFightingGameState.h"

AAndroidProjectPlayerController::AAndroidProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AAndroidProjectPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CurControlState = EUSControState::Fighting;
}

void AAndroidProjectPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	USGameState = GetWorld()->GetGameState<AUSFightingGameState>();


	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FString LogMessage = FString::Printf(TEXT("World URL: %s"), *GetWorld()->GetAddressURL());

	// UI Setting
	if (IsLocalController())
	{
		
		TouchInterfaceBtnWidgetInstance = Cast<UUSTouchInterfaceButton>(
			CreateWidget(GetWorld(), TouchInterfaceButtonWidgetAsset));
		TouchInterfaceBtnWidgetInstance->AddToViewport();

		PlayerStateHUDInstance = Cast<UUSPlayerCharacterHUD>(
			CreateWidget(GetWorld(), PlayerStateHUDAsset));
		PlayerStateHUDInstance->AddToViewport();

		SettingsWidgetInstance = CreateWidget(GetWorld(), SettingsWidgetAsset);
		SettingsWidgetInstance->AddToViewport();

		ActivateTouchInterface(TouchInterfaceJoystickAsset);


		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::JumpButton, EUSBtnTouchTriggerEvent::OnPressed
		                                            , this, &AAndroidProjectPlayerController::OnInputJump);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::Skill1Button, EUSBtnTouchTriggerEvent::OnPressed
		                                            , this, &AAndroidProjectPlayerController::OnInputSkill1);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::Skill2Button, EUSBtnTouchTriggerEvent::OnPressed
		                                            , this, &AAndroidProjectPlayerController::OnInputSkill2);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::AttackButton, EUSBtnTouchTriggerEvent::OnPressed
		                                            , this, &AAndroidProjectPlayerController::OnInputAttack);


		/** On Listen Server's side, OnRep_Pawn is called after PlayerController's BeginPlay */
		if (HasAuthority()) // for listen server
		{
			TouchInterfaceBtnWidgetInstance->BindPlayer(ControllingCharacter);
			PlayerStateHUDInstance->BindPlayer(ControllingCharacter);
		}
		else TryBindWidgetToCharacter();
		// In the case of Local Player, the call order of BeginPlay and OnRep_Pawn is not guaranteed
		// So Bind player wirh widget must be called both OnRep_Pawn and BeginPlay
	}
}

void AAndroidProjectPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*if(GetWorldTimerManager().TimerExists(RecalltimerHandle))
	{
		UE_LOG(LogTemp, Log, TEXT("Clear Timer"));
		GetWorldTimerManager().PauseTimer(RecalltimerHandle);
		GetWorldTimerManager().ClearTimer(RecalltimerHandle);
		
	}*/
	Super::EndPlay(EndPlayReason);
}

void AAndroidProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (CurControlState == EUSControState::Spectating)
	{
		if (!ObservingActor.IsValid())
		{
			FindNextObservableActor();
			SetViewTarget(ObservingActor.Get());
		}
	}
}


void AAndroidProjectPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (IsLocalController()) // On Possess works only on server. So It means Listen server's local PlayerController
		UpdatePawnInfo();
}

void AAndroidProjectPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	UE_LOG(LogTemp, Log, TEXT("OnRep: PawnName: %s"),
	       GetPawn() == nullptr ? TEXT("NULL") : *GetPawn()->GetClass()->GetName());
	if (GetPawn() == nullptr) return;

	UpdatePawnInfo();

	/** On Client's side, OnRep_Pawn is called after PlayerController's BeginPlay */
	if (IsLocalPlayerController())
	{
		TryBindWidgetToCharacter();
	}
}


void AAndroidProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
		check(EnhancedInputComponent != nullptr);

		// Setup mouse input event
		EnhancedInputComponent->BindAction(MyInputAction->IAJump, ETriggerEvent::Started, this
		                                   , &AAndroidProjectPlayerController::OnInputJump);

		EnhancedInputComponent->BindAction(MyInputAction->IASkill1, ETriggerEvent::Started, this
		                                   , &AAndroidProjectPlayerController::OnInputSkill1);

		EnhancedInputComponent->BindAction(MyInputAction->IASkill2, ETriggerEvent::Started, this
		                                   , &AAndroidProjectPlayerController::OnInputSkill2);

		EnhancedInputComponent->BindAction(MyInputAction->IAAttack, ETriggerEvent::Started, this
		                                   , &AAndroidProjectPlayerController::OnInputAttack);

		EnhancedInputComponent->BindAction(MyInputAction->IAMove, ETriggerEvent::Triggered, this
		                                   , &AAndroidProjectPlayerController::OnInputMove);

		EnhancedInputComponent->BindAction(MyInputAction->IAMove, ETriggerEvent::Completed, this
		                                   , &AAndroidProjectPlayerController::OffInputMove);
	}
}

void AAndroidProjectPlayerController::OnInputMove(const FInputActionValue& val)
{
	CurInputDir = val.Get<FVector2D>();
	CurInputDir.Normalize();

	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Move, CurInputDir));
}

void AAndroidProjectPlayerController::OffInputMove(const FInputActionValue& val)
{
	CurInputDir = FVector2D::Zero();
}

void AAndroidProjectPlayerController::OnInputJump(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Jump, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputSkill1(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Skill1, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputSkill2(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Skill2, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputAttack(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Attack, GetCurInputDir()));
}


bool AAndroidProjectPlayerController::TryBindWidgetToCharacter()
{
	if (IsValid(TouchInterfaceBtnWidgetInstance) && IsValid(ControllingCharacter))
	{
		TouchInterfaceBtnWidgetInstance->BindPlayer(ControllingCharacter);
	}
	else return false;

	if (IsValid(PlayerStateHUDInstance) && IsValid(ControllingCharacter))
	{
		PlayerStateHUDInstance->BindPlayer(ControllingCharacter);
	}
	else return false;
	
	return true;
}

void AAndroidProjectPlayerController::UpdatePawnInfo()
{
	ControllingCharacter = Cast<AUSFightingCharacter>(GetCharacter());
	if (ControllingCharacter)
	{
		ControllingCharacter->OnPlayerDestroyed.AddUniqueDynamic(
			this, &AAndroidProjectPlayerController::OnCharacterDestroyedOnClientCallback);
	}
}

void AAndroidProjectPlayerController::OnCharacterDestroyedOnClientCallback(AUSFightingCharacter* InUSFighter)
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->ClearBindingsForObject(this);

	TouchInterfaceBtnWidgetInstance->RemoveFromParent();
	TouchInterfaceBtnWidgetInstance->UnBindPlayer();
	TouchInterfaceBtnWidgetInstance = nullptr;

	PlayerStateHUDInstance->RemoveFromParent();
	PlayerStateHUDInstance->UnBindPlayer();
	PlayerStateHUDInstance = nullptr;

	ActivateTouchInterface(nullptr);
	TouchInterfaceJoystickAsset = nullptr;


	USMoveToSpectatingMode();
}


void AAndroidProjectPlayerController::OnInputTap(const FInputActionValue& val)
{
	FindNextObservableActor();
	SetViewTarget(ObservingActor.Get());
}

void AAndroidProjectPlayerController::FindNextObservableActor()
{
	CurrentObservingActorIdx++;

	if (USGameState == nullptr && GetWorld() != nullptr)
	{
		USGameState = GetWorld()->GetGameState<AUSFightingGameState>();
	} // Because USGameState must be replicated from server, It can be missing on client when cache from BeginPlay

	if (USGameState->GetActiveCharacterCount() <= CurrentObservingActorIdx)
	{
		CurrentObservingActorIdx = -1; // Idx -1 means OverviewObservingActor
	}
	ObservingActor = USGameState->GetActorToView(CurrentObservingActorIdx);
}

void AAndroidProjectPlayerController::USMoveToSpectatingMode()
{
	if (!IsValid(this)) return;
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->
		BindAction(MyInputAction->IATap, ETriggerEvent::Started, this
		           , &AAndroidProjectPlayerController::OnInputTap);

	FindNextObservableActor();
	SetViewTarget(ObservingActor.Get());

	SetActorTickEnabled(true);
	CurControlState = EUSControState::Spectating;
}
