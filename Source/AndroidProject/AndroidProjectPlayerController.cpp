// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroidProjectPlayerController.h"
#include "GameFramework/Pawn.h"
#include "AndroidProjectCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputDataAsset/MyDataAsset.h"
#include "Blueprint/UserWidget.h"

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
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FString LogMessage = FString::Printf(TEXT("World URL: %s"), *GetWorld()->GetAddressURL());
	GEngine->AddOnScreenDebugMessage(10, 10.f, FColor::Red, LogMessage);

	UE_LOG(LogTemp, Log, TEXT("loca: %d, remote: %d"), GetLocalRole(), GetRemoteRole());
	if(	IsLocalController() )
	{
		ServerClientWidgetInstance = Cast<UUserWidget>(CreateWidget(GetWorld(), ServerClientWidget));
		ServerClientWidgetInstance->AddToViewport();
	}
}

void AAndroidProjectPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if( IsLocalController() )
	{
		ServerClientWidgetInstance->RemoveFromParent();
	}
}


void AAndroidProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();


	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(MyInputAction->IAVJoystickMovement, ETriggerEvent::Triggered, this, &AAndroidProjectPlayerController::OnInputMove);
		EnhancedInputComponent->BindAction(MyInputAction->IAVScreenTouch, ETriggerEvent::Triggered, this, &AAndroidProjectPlayerController::OnInputMoveTouch);
	}
}


void AAndroidProjectPlayerController::OnInputMove(const FInputActionValue& val)
{

	
	AAndroidProjectCharacter* myCharacter = Cast<AAndroidProjectCharacter>(GetCharacter());

	if(myCharacter)
	{
		FVector2D dir2D = val.Get<FVector2D>();
		UE_LOG(LogTemp, Log, TEXT("x: %lf, y: %lf"), dir2D.X, dir2D.Y);
		myCharacter->MoveTo(FVector(dir2D.Y, dir2D.X, 0));
	}
}

void AAndroidProjectPlayerController::OnInputMoveTouch(const FInputActionValue& val)
{
	
	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
	AAndroidProjectCharacter* MyCharacter = Cast<AAndroidProjectCharacter>(GetCharacter());
	
	if(bHitSuccessful)
	{
		destination = Hit.Location;
		MyCharacter->MoveTo(destination - MyCharacter->GetActorLocation());
	}
	else
	{
		destination = MyCharacter->GetActorLocation();
	}
}
