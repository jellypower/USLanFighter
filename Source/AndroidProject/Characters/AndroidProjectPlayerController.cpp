// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InputDataAsset/InputActionDataAsset.h"

#include "AndroidProjectCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/USTouchInterfaceButton.h"



AAndroidProjectPlayerController::AAndroidProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AAndroidProjectPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FString LogMessage = FString::Printf(TEXT("World URL: %s"), *GetWorld()->GetAddressURL());
	//GEngine->AddOnScreenDebugMessage(10, 10.f, FColor::Red, LogMessage);

	UE_LOG(LogTemp, Log, TEXT("loca: %d, remote: %d"), GetLocalRole(), GetRemoteRole());

	// UI Setting
	if (IsLocalController())
	{
		ServerClientWidgetInstance = Cast<UUserWidget>(CreateWidget(GetWorld(), ServerClientWidgetAsset));
		ServerClientWidgetInstance->AddToViewport();

		TouchInterfaceBtnWidgetInstance = Cast<UUSTouchInterfaceButton>(
			CreateWidget(GetWorld(), TouchInterfaceButtonWidgetAsset));
		TouchInterfaceBtnWidgetInstance->AddToViewport();

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::JumpButton, EUSBtnTouchTriggerEvent::OnPressed
			, this, &AAndroidProjectPlayerController::OnInputJump);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::Skill1Button, EUSBtnTouchTriggerEvent::OnPressed
			, this, &AAndroidProjectPlayerController::OnInputSkill1);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::Skill2Button, EUSBtnTouchTriggerEvent::OnPressed
			, this, &AAndroidProjectPlayerController::OnInputSkill2);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::AttackButton, EUSBtnTouchTriggerEvent::OnPressed
			, this, &AAndroidProjectPlayerController::OnInputAttack);

		TouchInterfaceBtnWidgetInstance->BindAction(EUSButtonType::KickButton, EUSBtnTouchTriggerEvent::OnPressed
			, this, &AAndroidProjectPlayerController::OnInputKick);

		
	}

}

void AAndroidProjectPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControllingCharacter = CastChecked<AAndroidProjectCharacter>(InPawn);
}

void AAndroidProjectPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	ControllingCharacter = CastChecked<AAndroidProjectCharacter>(GetCharacter());
}


void AAndroidProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
		check(EnhancedInputComponent != nullptr);

		// Setup mouse input events
		EnhancedInputComponent->BindAction(MyInputAction->IAMove, ETriggerEvent::Triggered, this
			, &AAndroidProjectPlayerController::OnInputMove);

		EnhancedInputComponent->BindAction(MyInputAction->IAJump, ETriggerEvent::Started, this
			, &AAndroidProjectPlayerController::OnInputJump);
		
		EnhancedInputComponent->BindAction(MyInputAction->IASkill1, ETriggerEvent::Started, this
			, &AAndroidProjectPlayerController::OnInputSkill1);

		EnhancedInputComponent->BindAction(MyInputAction->IASkill2, ETriggerEvent::Started, this
			, &AAndroidProjectPlayerController::OnInputSkill2);

		EnhancedInputComponent->BindAction(MyInputAction->IAAttack, ETriggerEvent::Started, this
			, &AAndroidProjectPlayerController::OnInputAttack);

		EnhancedInputComponent->BindAction(MyInputAction->IAKick, ETriggerEvent::Started, this
			, &AAndroidProjectPlayerController::OnInputKick);
	}
}

void AAndroidProjectPlayerController::OnInputMove(const FInputActionValue& val)
{
	FVector2D dir2D = val.Get<FVector2D>();
	dir2D.Normalize();
	float NormalVecY = dir2D.Y;
	float NormalVecX = dir2D.X;
	
	ControllingCharacter->OrderToMove(FVector(dir2D.X, dir2D.Y, 0));
	
}

void AAndroidProjectPlayerController::OnInputJump(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Log, TEXT("Jump!"));
	ControllingCharacter->OrderToJump();
}

void AAndroidProjectPlayerController::OnInputSkill1(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Log, TEXT("Skill1"));
}

void AAndroidProjectPlayerController::OnInputSkill2(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Log, TEXT("Skill2"));

}

void AAndroidProjectPlayerController::OnInputAttack(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Log, TEXT("Attack"));

}

void AAndroidProjectPlayerController::OnInputKick(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Log, TEXT("Kick"));

}

