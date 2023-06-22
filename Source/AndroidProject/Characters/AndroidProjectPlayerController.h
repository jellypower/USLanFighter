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

#pragma region Unreal Events
public:
	AAndroidProjectPlayerController();
	
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override; // works only server
	virtual void OnRep_Pawn() override;

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
	void OnInputSmash(const FInputActionValue& val);

	UPROPERTY(VisibleAnywhere, Category=Input)
	FVector2D CurInputDir;

#pragma endregion
	
#pragma region Widget variables
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ServerClientWidgetAsset;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UUserWidget> ServerClientWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TouchInterfaceButtonWidgetAsset;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UUSTouchInterfaceButton> TouchInterfaceBtnWidgetInstance;

#pragma endregion


#pragma region Character Control code
private:
	/** GetCharcter()'s cache */
	UPROPERTY()
	TObjectPtr<class AUSFightingCharacter> ControllingCharacter;



#pragma endregion

};


