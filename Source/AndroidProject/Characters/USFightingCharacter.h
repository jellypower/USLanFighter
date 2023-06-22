// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "USCharacterAnim.h"
#include "GameFramework/Character.h"
#include "CharacterShare/EUSPlayerActionState.h"
#include "CharacterShare/USOrder.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/USWeaponBase.h"

#include "USFightingCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCastNotCastableSkill, bool, CastState, FString, SkillName);

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

	UPROPERTY(VisibleAnywhere, Category = CharacterStat)
	UCapsuleComponent* DmgBox;
	
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* NameTagWidget;

	UPROPERTY(EditDefaultsOnly, Category= Weapon)
	class USceneComponent* WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category= Weapon)
	class USceneComponent* SubWeaponSocket;

public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

#pragma endregion


#pragma region USFightingCharacter Action

public:
	virtual void OnJumped_Implementation() override;
	virtual bool CanJumpInternal_Implementation() const override;

	virtual bool IsMoveInputIgnored() const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;
	float TakeImpact(float ImpactAmount, AController* EventInstigator, AActor* DamageCauser, const FVector2D& AtkDir);

	void OrderTo(FUSOrder InOrder);


	/** Mostly called by USCharacterAnim's Animnotify */
	void FinishCasting();
	void TriggerSkillEffect();

	void InterruptCasting();

	FOnCastNotCastableSkill OnCastNotCastableSkillOnClient;
	
protected:
	/** Use only local*/
	UPROPERTY(VisibleInstanceOnly)
	FUSOrder BufferedOrder;

	/** Use Only Server*/
	UPROPERTY(VisibleInstanceOnly)
	FUSOrder ServerBufferedOrder;

private:
	UFUNCTION(Server, Reliable)
	void SendOrderToServer_Reliable(FUSOrder InOrder);

	void StartCastProcessOnServer();
	void ExecuteAttackOnServer(const FVector2D& Dir);
	void ExecuteSkillOnServer(uint8 skillIdx,const FVector2D& Dir);

	UFUNCTION(NetMulticast, Reliable)
	void AnimateAttack(const uint8 InCurComboNum);

	UFUNCTION(NetMulticast, Reliable)
	void StopAnimateAttack();

	UFUNCTION(NetMulticast, Reliable)
	void StopAnimateAllCastingMotion();

	UFUNCTION(NetMulticast, Reliable)
	void AnimateSkillCasting(uint8 skillIdx);

	UFUNCTION(NetMulticast, Reliable)
	void AnimateImpacted(uint8 AnimateIdx);

	/** Judging must always take place in server  */
	void EndCastProcessOnServer(bool bIsInterrupt);

	void SendOrderToServer(const FUSOrder& InOrder);
	void ExecuteOrderOnServer(const FUSOrder& InOrder);

	/** Continuously inspect Character's state. It adjust the state.
	 * It can block the abnormal state that ActionStateBitMasks's replication packet loss.
	 * Property replication is reliable but It is reliable only for the final result
	 * https://vorixo.github.io/devtricks/atomicity/
	 * So prediction must occur.
	 * 
	 * This function is for the adjust. Basic state update must be occured with character's action.
	 */
	void SimulateStateUpdate();

#pragma endregion


#pragma region Character Stat

protected:
	UPROPERTY(Replicated)
	FString CharacterName;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CharacterStat)
	float BasicJumpIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStat)
	float BasicWalkSpeed;

	UPROPERTY(EditAnywhere, Replicated)
	float MaxHP;

public:
	FString GetCharacterName() const { return CharacterName; }
	void SetCharacterName(const FString& name) { this->CharacterName = name; }

	/**
	 * This function must be called by Server
	 */
	void HandWeaponToPlayer(UClass* InWeaponClass);


	float GetMaxHP() const { return MaxHP; }

protected:
	void EquipWeapon();

#pragma endregion


#pragma region Character State

private:
	UPROPERTY(ReplicatedUsing=OnRep_CurEquippedWeapon)
	AUSWeaponBase* CurEquippedWeapon = nullptr;

	UFUNCTION()
	void OnRep_CurEquippedWeapon();

	UPROPERTY()
	class UUSCharacterAnim* CharAnim;

	UPROPERTY(Replicated)
	float CurHP;

	UPROPERTY(Replicated)
	uint8 CurComboIdx = 0;

	TWeakObjectPtr<class USkillComponentBase> CurCastingSkill;
	

	UPROPERTY(Replicated, VisibleInstanceOnly,
		Meta = (Bitmask, BitmaskEnum = "EUSPlayerActionState"), Category = "Player State")
	uint32 ActionStateBitMask = 0;

public:
	uint8 IsOrderExecutableState() const
	{
		return (ActionStateBitMask & EUSPlayerActionStateValue::OrderUnAcceptableBitMask) == 0;
	}


	uint8 IsOrderIgnoredOnServer(const FUSOrder& InOrder) const 
	{
		switch(InOrder.Type)
		{
		case FUSOrderType::Attack:
			if (IsCasting() && CurComboIdx == GetCurEquippedWeapon()->GetComboMaxNum() - 1)
			{
				return true;
			}
		}

		return false;
	}

	uint8 IsInputIgnoredToImproveSenseOfOperation(const FUSOrder& InOrder) const
	{
		switch (InOrder.Type)
		{
		case FUSOrderType::Jump:
			if (IsJumping()) return true;
			break;

		case FUSOrderType::Move:
			if (IsCasting()) return true;
			if (BufferedOrder.Type == FUSOrderType::Attack) return true;
			break;

		case FUSOrderType::Attack:
			if (IsCasting() && CurComboIdx == GetCurEquippedWeapon()->GetComboMaxNum() - 1)
			{
				return true;
			}
		}

		return false;
	}

	bool IsJumping() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Jump);
	}

	bool IsMoving() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Move);
	}

	bool IsCasting() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Cast);
	}

	bool IsImpacted() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Impacted);
	}

	bool IsBlown() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Blown);
	}

	void RecoveryFromImpactedOnServer()
	{
		UE_LOG(LogTemp, Log, TEXT("Authority: %d"), GetLocalRole() == ROLE_Authority);
		if(HasAuthority())
		{
			UE_LOG(LogTemp, Log, TEXT("Recovery Implementation"));
			ActionStateBitMask = ActionStateBitMask & ~EUSPlayerActionState::Impacted;
		}
	}


	float GetCurHP() const { return CurHP; }
	AUSWeaponBase* GetCurEquippedWeapon() const { return CurEquippedWeapon; }

#pragma endregion


#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)

private
:
	void PrintCharacterStateOnScreen() const
	{
		GEngine->AddOnScreenDebugMessage(11, 10.f, FColor::Blue,
		                                 FString::Printf(TEXT(
			                                 "OrderAcceptable: %d\n Move: %d\n Jump: %d\n IsAttacking: %d\n "
			                                 "IsImpacted: %d\n IsMoveInputIgnored: %d"),
		                                                 IsOrderExecutableState(), IsMoving(), IsJumping(),
		                                                 IsCasting(),
		                                                 IsImpacted(),
		                                                 IsMoveInputIgnored()
		                                                 ));
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
