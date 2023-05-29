// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AndroidProjectCharacter.generated.h"



UCLASS(Blueprintable)
class AAndroidProjectCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Events
public:
	AAndroidProjectCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;


#pragma endregion


#pragma region Actor components
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* NameTagWidget;
#pragma endregion

#pragma region Actor components GetSet
public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

#pragma endregion



#pragma region AndroidProjectCharacter override functions
public:
	virtual bool IsMoveInputIgnored() const override;

#pragma endregion

#pragma region AndroidProjectCharacter Custom functions
public:
	void OrderToMove(const FVector& dir);

	void OrderToJump();

#pragma endregion


#pragma region Character Stat
private:
	FString characterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= CharacterStat,meta = (AllowPrivateAccess = "true"))
	float BasicJumpIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStat, meta = (AllowPrivateAccess = "true"))
	float BasicWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStat, meta = (AllowPrivateAccess = "true"))
	float AirWalkAccel;
#pragma endregion

#pragma region Character Stat GetSet
public:
	FString GetCharacterName() const { return characterName;}
	void SetCharacterName(const FString& name) { this->characterName = name;}

#pragma endregion


#pragma region Character State	
private:
	

#pragma endregion
	
};

