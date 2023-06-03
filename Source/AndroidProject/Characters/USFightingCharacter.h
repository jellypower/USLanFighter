// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterShare/EUSPlayerActionState.h"
#include "CharacterShare/USOrder.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "USFightingCharacter.generated.h"


class AUSWeaponBase;
UCLASS(Blueprintable)
class AUSFightingCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Events

public:
	AUSFightingCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	UPROPERTY(EditDefaultsOnly, Category= Weapon)
	class USceneComponent* WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category= Weapon)
	class USceneComponent* ShieldSocket;

public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

#pragma endregion


#pragma region USFightingCharacter Init
protected:
	void EquipWeapon(UClass* InWeaponClass);

#pragma endregion 

#pragma region USFightingCharacter Action

public:
	FUSOrder BufferedOrder;

	virtual void OnJumped_Implementation() override;
	virtual bool IsMoveInputIgnored() const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

	float TakeImpact();
	void OrderTo(FUSOrder InOrder);

	UFUNCTION(Server, Reliable)
	void Attack();

private:
	inline void ExecuteOrder(const FUSOrder& InOrder);


#pragma endregion


#pragma region Character Stat

protected:
	UPROPERTY(Replicated)
	FString CharacterName;

	UPROPERTY(EditAnywhere, Category= CharacterStat)
	TSubclassOf<AUSWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CharacterStat)
	float BasicJumpIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStat)
	float BasicWalkSpeed;

	UPROPERTY(EditAnywhere, Replicated)
	float MaxHP;

public:
	FString GetCharacterName() const { return CharacterName; }
	void SetCharacterName(const FString& name) { this->CharacterName = name; }

	float GetMaxHP() const { return MaxHP; }
#pragma endregion


#pragma region Character State

private:

	UPROPERTY()
	AUSWeaponBase* CurEquippedWeapon;
	
	UPROPERTY(Replicated)
	float CurHP;

	UPROPERTY(Replicated, VisibleInstanceOnly,
		Meta = (Bitmask, BitmaskEnum = "EUSPlayerActionState"), Category = "Player State")
	uint8 ActionStateBitMask = 0; // TO-DO: state update 구문 넣어주기


public:
	inline int8 IsOrderAcceptable() const;
	inline int8 IsNextComboAtkAcceptable() const;
	inline int8 IsJumping() const;
	inline int8 IsMoving() const;

	float GetCurHP() const { return CurHP; }

#pragma endregion


#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)

private:
	void PrintCharacterStateOnScreen() const
	{
		GEngine->AddOnScreenDebugMessage(11, 10.f, FColor::Blue,
		                                 FString::Printf(TEXT("OrderAcceptable: %d\n Move: %d\n Jump: %d\n "),
		                                                 IsOrderAcceptable(), IsMoving(), IsJumping()));
	}

	void PrintCharacterStatOnScreen() const
	{
		GEngine->AddOnScreenDebugMessage(12, 10.f, FColor::Blue,
		                                 FString::Printf(
			                                 TEXT("PlayerName: %s\n BasicJumpIntensity: %f\n BasicWalkSpeed: %f\n "),
			                                 *GetCharacterName(),
			                                 GetCharacterMovement()->JumpZVelocity,
			                                 GetCharacterMovement()->MaxWalkSpeed));
	}

#endif
}; // End Of Class
