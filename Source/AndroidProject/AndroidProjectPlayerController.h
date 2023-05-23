// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "AndroidProjectPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class AAndroidProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAndroidProjectPlayerController();
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UMyDataAsset* MyInputAction;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> ServerClientWidget;

	UPROPERTY()
	class UUserWidget* ServerClientWidgetInstance;

protected:

	FVector_NetQuantize destination;

	virtual void SetupInputComponent() override;
	// To add mapping context
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Input handlers for SetDestination action. */
	void OnInputMove(const FInputActionValue& val);

	void OnInputMoveTouch(const FInputActionValue& val);


};


