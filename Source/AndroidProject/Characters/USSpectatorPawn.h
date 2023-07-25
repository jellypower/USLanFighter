// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "USSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class ANDROIDPROJECT_API AUSSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
public:
	AUSSpectatorPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	


public:
	void MoveToNextObservableActor();
	const class USpringArmComponent* GetCameraBoom() const{ return CameraBoom; }

private:

	uint8 CurrentObservingActorIdx = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	const AActor* ObservingActor;

	class AUSFightingGameState* USGameState;

	
	void FindNextObservableActor_internal();
};
