// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "USCharacterAnim.h"
#include "GameFramework/Character.h"
#include "CharacterShare/EUSPlayerActionState.h"
#include "CharacterShare/USOrder.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/WeakInterfacePtr.h"
#include "Weapons/USWeaponBase.h"
#include "CharacterShare/ShareEnums.h"

#include "USFightingCharacter.generated.h"


class IUSAttackBlockable;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCastNotCastableSkill, bool, CastState, FString, SkillName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDied, AUSFightingCharacter*, Character, ECauseOfDeath, InCauseOfDeath, float, RecallTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRecall, AUSFightingCharacter*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeactivated, AUSFightingCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActivated, AUSFightingCharacter*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDestroyed, AUSFightingCharacter*, InUSFighter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIdentityColorApply, FColor, InColor);



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEquipWeapon, AUSWeaponBase*, Weapon);

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


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

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

	UFUNCTION()
	float USTakeDamage
		(const float DamageAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter ,UObject* DamageCauser, const FVector2D& AtkDir);
	float USTakeImpact
		(const float ImpactAmount, AController* EventInstigator, AUSFightingCharacter* DmgCausedFighter, UObject* DamageCauser, const FVector2D& AtkDir);

	void EnterInvincible(float InTime);

	UFUNCTION(BlueprintCallable)
	bool USIsDied();

	UFUNCTION(BlueprintCallable)
	void USKillPlayerOnServer(ECauseOfDeath InCauseOfDeath, float RecallTime);
	
	UFUNCTION(BlueprintCallable)
	void USRecallPlayerOnServer(AActor* RecallSpot,float InInvincibleTime);


	
	void OrderTo(FUSOrder InOrder);


	/** Mostly called by USCharacterAnim's Animnotify */
	void FinishCasting();
	void TriggerSkillEffect();

	void InterruptCastingOnServer();

	mutable FOnCastNotCastableSkill OnCastNotCastableSkillOnClient;

	mutable FOnPlayerDied OnPlayerDiedOnServer;
	mutable FOnPlayerRecall OnPlayerRecallOnServer;

	mutable FOnPlayerActivated OnPlayerActivated;
	mutable FOnPlayerDeactivated OnPlayerDeactivated;
	mutable FOnPlayerDestroyed OnPlayerDestroyed;

	mutable FOnPlayerEquipWeapon OnEquipWeapon;

	mutable FOnIdentityColorApply OnIdentityColorApply;
	
protected:
	/** Use only local*/
	UPROPERTY(VisibleInstanceOnly)
	FUSOrder BufferedOrder;

	/** Use Only Server*/
	UPROPERTY(VisibleInstanceOnly)
	FUSOrder ServerBufferedOrder;

private:


	UFUNCTION(NetMulticast, Reliable)
	void USDeactivatePlayer(bool bHiddenPlayer, bool bFixLocation = false);

	UFUNCTION(NetMulticast, Reliable)
	void USReActivatePlayer();

	
	UFUNCTION(Server, Reliable)
	void SendOrderToServer_Internal_Reliable(FUSOrder InOrder);

	void StartCastProcessOnServer();
	void ExecuteAttackOnServer(const FVector2D& InDir);
	void ExecuteSkillOnServer(uint8 skillIdx,const FVector2D& InDir);

	AUSFightingCharacter* DetectClosestTargetRadiusBase(float Radius) const;
	AUSFightingCharacter* DetectClosestTargetRadiusDegreeBase(FVector2D NormalizedAtkDir ,float Radius, float ThresholdDegree) const;

	UFUNCTION(NetMulticast, Reliable)
	void AnimateAttack(const uint8 InCurComboNum, FVector_NetQuantizeNormal CastDir);

	UFUNCTION(NetMulticast, Reliable)
	void StopAnimateAttack();

	UFUNCTION(NetMulticast, Reliable)
	void StopAnimateAnyMotion();

	UFUNCTION(NetMulticast, Reliable)
	void AnimateSkillCasting(uint8 skillIdx, FVector_NetQuantizeNormal CastDir);

	UFUNCTION(NetMulticast, Reliable)
	void AnimateImpacted(uint8 AnimateIdx);

	UFUNCTION(NetMulticast, Reliable)
	void AnimateBlown(FVector_NetQuantizeNormal CastDir);

	UFUNCTION(NetMulticast, Unreliable)
	void CastDoubleJumpEffect();

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
	void SimulateStateUpdateOnServer(float DeltaTime);

	void GlobalVisualUpdateOfMesh(float DeltaTime);

#pragma endregion


#pragma region Character Stat

protected:
	UPROPERTY(Replicated)
	FString CharacterName;

	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 CharacterNetIdx;


	UPROPERTY(EditAnywhere, ReplicatedUsing= OnRep_JumpIntensity, BlueprintReadWrite, Category= CharacterStat)
	float JumpIntensity;

	UFUNCTION()
	void OnRep_JumpIntensity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStat)
	float BasicWalkSpeed;


	UPROPERTY(EditAnywhere, Replicated)
	float MaxHP;


public:
	FString GetCharacterName() const { return CharacterName; }
	void SetCharacterName(const FString& name) { this->CharacterName = name; }

	uint8 GetPlayerNetIdx() const { return CharacterNetIdx; }
	void SetPlayerNetIdx(const uint8 InNetIdx) { CharacterNetIdx = InNetIdx;}
	
	/** This function must be called by Server */
	void HandWeaponToPlayer(UClass* InWeaponClass);

	/** This function must be called by server */
	void SetPlayerIdentityColorOnServer(FColor color);

	const FColor GetPlayerIdentityColor() const { return PlayerIdentityColor; }

	float GetMaxHP() const { return MaxHP; }

protected:
	void EquipWeapon();
	void ApplyIdentityColor();
	void ApplyJumpIntensity();

#pragma endregion


#pragma region Character State

private:
	UPROPERTY(ReplicatedUsing=OnRep_CurEquippedWeapon)
	AUSWeaponBase* CurEquippedWeapon = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerIdentityColor)
	FColor PlayerIdentityColor;

	UPROPERTY(EditDefaultsOnly, Category=PlayerVisual)
	class UNiagaraSystem* DoubleJumpEffectNiagara;

	UPROPERTY()
	UMaterialInstanceDynamic* IdentityMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* BlinkingMaterial;
	
	UFUNCTION()
	void OnRep_CurEquippedWeapon();

	UFUNCTION()
	void OnRep_PlayerIdentityColor();
	
	TArray<TWeakInterfacePtr<IUSAttackBlockable>> AttackBlockingSkills;
	
	
	UPROPERTY()
	class UUSCharacterAnim* CharAnim;

	UPROPERTY(Replicated)
	uint8 PlayerLifeCount;

	UPROPERTY(Replicated)
	float CurHP;

	UPROPERTY(Replicated)
	uint8 CurComboIdx = 0;


	bool bIsFallingPrevFrame = false;
	bool bIsGroundedThisFrame = false;
	


	TWeakObjectPtr<class USkillComponentBase> CurCastingSkill;
	

	UPROPERTY(Replicated, VisibleInstanceOnly,
		Meta = (Bitmask, BitmaskEnum = "EUSPlayerActionState"), Category = "Player State")
	uint32 ActionStateBitMask = 0;

	UPROPERTY(EditDefaultsOnly)
	float DefaultImpactTime = 0.67f;

	UPROPERTY(VisibleInstanceOnly, Replicated)
	float ImpactTimer = 0;

	UPROPERTY(EditDefaultsOnly)
	float DefaultBlownRecoveryTime = 1 + 0.43;

	UPROPERTY(VisibleInstanceOnly, Replicated)
	float BlownRecoveryTimer = 0;
	
	UPROPERTY(VisibleInstanceOnly, Replicated)
	float InvincibleTimer = 0;

	UPROPERTY(VisibleInstanceOnly, Replicated)
	float InvincibleTime = 0;


public:
	uint8 IsOrderExecutableState() const
	{
		return (ActionStateBitMask & EUSPlayerActionStateValue::OrderUnAcceptableBitMask) == 0;
	}

	bool IsExecutableOrderInOrderNotExecutableState(const FUSOrder& InOrder) const;


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
			if (IsJumping() && !CanJump()) return true;
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

	bool IsInvincible() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Invincible);
	}

	bool IsDied() const
	{
		return (ActionStateBitMask & EUSPlayerActionState::Died);
	}
	
	

	bool GetIsGroundedThisFrame() const
	{
		return bIsGroundedThisFrame;
	}


	void RecoveryFromImpacted();
	void RecoveryFromBlown();
	void RecoveryFromCast();
	void RecoveryFromInvincible();
	

	float GetCurHP() const { return CurHP; }

	void SetPlayerLifeCount(uint8 InLifeCount) { PlayerLifeCount = InLifeCount; }
	uint8 GetCurLifeCount() const { return PlayerLifeCount; }
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
			                                 "IsImpacted: %d\n IsBlown: %d\n IsInvincible: %d\n IsInAir: %d\n"\
											 ),
		                                                 IsOrderExecutableState(), IsMoving(), IsJumping(),
		                                                 IsCasting(),
		                                                 IsImpacted(),
		                                                 IsBlown(),
		                                                 IsInvincible(),
		                                                 GetCharacterMovement()->IsFalling()
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
