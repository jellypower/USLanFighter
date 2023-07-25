// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "AndroidProjectPlayerController.generated.h"


enum class EUSControState
{
	Fighting,
	Spectating
};

UCLASS()
class AAndroidProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region Unreal Events
public:
	AAndroidProjectPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PlayerTick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override; // works only server
	virtual void OnRep_Pawn() override; // works only on client


#pragma endregion
	
#pragma region Input Settings variables
public:
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputActionDataAsset* MyInputAction;

	FVector2D GetCurInputDir() const { return CurInputDir; }

protected:
	
	void OnInputMove(const FInputActionValue& val);
	void OffInputMove(const FInputActionValue& val);
	void OnInputJump(const FInputActionValue& val);
	void OnInputSkill1(const FInputActionValue& val);
	void OnInputSkill2(const FInputActionValue& val);
	void OnInputAttack(const FInputActionValue& val);

	UPROPERTY(VisibleAnywhere, Category=Input)
	FVector2D CurInputDir;

#pragma endregion
	
#pragma region Widget variables
private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUSPlayerCharacterHUD> PlayerStateHUDAsset;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UUSPlayerCharacterHUD> PlayerStateHUDInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TouchInterfaceButtonWidgetAsset;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UUSTouchInterfaceButton> TouchInterfaceBtnWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> SettingsWidgetAsset;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UUserWidget> SettingsWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	class UTouchInterface* TouchInterfaceJoystickAsset;

	bool TryBindWidgetToCharacter();
	
#pragma endregion


#pragma region Character Control code
private:
	/** GetCharcter()'s cache */
	UPROPERTY()
	class AUSFightingCharacter* ControllingCharacter;

	void UpdatePawnInfo();

	UFUNCTION()
	void OnCharacterDestroyedOnClientCallback(AUSFightingCharacter* InUSFighter);


#pragma endregion


#pragma region Spectator Control Code
private:
	TWeakObjectPtr<class AUSFightingGameState> USGameState;
	EUSControState CurControlState;

	TWeakObjectPtr<AActor> ObservingActor;
	uint8 CurrentObservingActorIdx = -1;

	void FindNextObservableActor();
	UFUNCTION()
	void USMoveToSpectatingMode();
	void OnInputTap(const FInputActionValue& val);

#pragma endregion 
	
};


