# USLanFighter

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/USLANFighterPlayVideoBanner.png" alt="drawing" width="800"/>
</p>

> Smilegate주최 부트캠프 UNSEEN 교육과정을 통해 제작한 1인 프로젝트
> 

# [게임 소개]


- 게임 제목: Ultra Super Lan Fighter(=USLANFighter)
- 플랫폼: 안드로이드, 언리얼엔진5
- 레퍼런스 게임: 슈퍼 스매시 브라더스 얼티밋, 스매시 레전드
- 게임 컨셉:
    - 다양한 무기와 스킬들을 활용해 상대방을 물리치고 전장에서 살아남으세요!
    - 모바일 LAN환경에서 친구와 함께 플레이 가능한 모바일 대난투 액션게임!

## 다양한 무기

## 다양한 스킬

## 상대를 그로기 상태로 만들고 날려버리기!

## 상대의 허점을 노리기!

# [기술적 주안점]

## 1. 언리얼 엔진 아키텍처


- 베이스 클래스 인터페이스를 공유하는 다양한 블루프린트 클래스 무기와, 스킬들
- C++클래스에서 기본적인 무기의 메쉬, 동작방식, 공격 모션 등의 필요한 데이터를 정의하고, 블루프린트에서 이러한 내용들을 상속받아 편집 가능케 하여 코드 재사용성과 무기의 제작효율을 높임

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/WeaponInterface.png" alt="drawing" width="600"/>
</p>
- 소스코드 링크:
    - Base Class: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Weapons/USWeaponBase.h
    - Weapon Class Folder: https://github.com/jellypower/USLanFighter/tree/master/Source/AndroidProject

  
---
- 스킬을 컴포넌트로 제작하고 베이스 클래스를 상속받도록 하여 인터페이스만 공통되면 스크립트가 아닌 에디터를 통해 손쉽게 스킬을 장착, 해제 가능케 함
- 스킬캐스팅 과정을 캐스팅 시작, 캐스팅 완료, 캐스팅 방해의 형태로 인터페이스화하여 스킬 제작 효율성과 코드 재사용성을 높임
- 스킬 캐스팅 과정에 영향을 주는 추가적인 기능들은 별도의 인터페이스를 제작해 확장성을 높임

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/SkillInterface.png" alt="drawing" width="600"/>
</p>


- 소스코드 링크:
    - Skill Base Class: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Skill/SkillComponentBase.h
    - Skill Class Folder: https://github.com/jellypower/USLanFighter/tree/master/Source/AndroidProject/Skill

## 2. UI아키텍쳐


- 인게임 전투 레이어와 UI/입력 레이어를 명확히 구분해 전투 레이어가 UI/입력부분에 어떠한 영향도 끼치지 않도록 하여 소스코드간 종속성을 없앰


<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/UIGameLayer.png" alt="drawing" width="600"/>
</p>

  
---
- 입력부를 PlayerController에 종속시키고 입력을 명령의 형태로 변환하여 플레이어에게 전달함.
- 이를 통해 플레이어의 입력을 관리하는 기능과 캐릭터의 움직임관리하는 기능을 분리하여 확장성을 높임

```cpp
UENUM(BlueprintType)
enum class FUSOrderType : uint8
{
	None = 0,
	Move,
	Jump,
	Attack,
	Skill1,
	Skill2,
	Smash
};

USTRUCT(BlueprintType)
struct FUSOrder
{
.
.
.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUSOrderType Type;

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal dir;
.
.
.
};
```
소스코드 링크: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/CharacterShare/USOrder.h

```cpp
.
.
.
void AAndroidProjectPlayerController::OnInputMove(const FInputActionValue& val)
{
	CurInputDir = val.Get<FVector2D>();
	CurInputDir.Normalize();

	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Move, CurInputDir));
}

void AAndroidProjectPlayerController::OffInputMove(const FInputActionValue& val)
{
	CurInputDir = FVector2D::Zero();
}

void AAndroidProjectPlayerController::OnInputJump(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Jump, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputSkill1(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Skill1, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputSkill2(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Skill2, GetCurInputDir()));
}

void AAndroidProjectPlayerController::OnInputAttack(const FInputActionValue& val)
{
	ControllingCharacter->OrderTo(FUSOrder(FUSOrderType::Attack, GetCurInputDir()));
}

.
.
.
```
소스코드 링크: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Characters/AndroidProjectPlayerController.cpp 

  
---
- UI가 인게임으로부터 제공받아야 하는 데이터, 기능들은 C++클래스에 종속시킨 이후 리플렉션 시스템을 통해 블루프린트 클래스에 제공
- UI가 실제 화면에서 보여지고 플레이어와 상호작용 하는 기능들은 블루프린트 클래스를 통해 구현하여 디자인변경과 그래픽변경에서 발생 할 수 있는 사이드이펙트를 감소시킴
<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/CharacterUIBlueprint.png" alt="drawing" width="600"/>
</p>

- 소스코드 링크:
    - Cpp 기반:  https://github.com/jellypower/USLanFighter/tree/master/Source/AndroidProject/UI
    - Blueprint 기반: https://github.com/jellypower/USLanFighter/tree/master/Content/UI/InGameFightingUI

  
---

- 모바일 환경에서 캐릭터 조작용 위젯에 입력을 바인딩하는 인터페이스를 언리얼 내장 InputComponent에 입력을 바인딩 하는 인터페이스와 유사하게 만들어 입력을 규격화하여 코드의 가독성을 높임
- UI가 캐릭터의 정보에 접근해야 하는 경우 항상 `const pointer`를 활용하여 캐릭터의 상태를 직접적으로 변화시키는 일이 없도록 격리
    - 캐릭터 조작용 위젯의 경우 `Input Component`와 동일하게 `Player Controller`에서 콜백 함수를 넘겨주는 방식으로 작동시켜 입력과 실제 액션을 가상화 하였음

```cpp
.
.
.

UCLASS()
class ANDROIDPROJECT_API UUSTouchInterfaceButton : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY()
	const class AUSFightingCharacter* ControllingCharacter;
	UPROPERTY()
	const class USkillComponentBase* Skill1;
	UPROPERTY()
	const class USkillComponentBase* Skill2;
	
	FButtonEventDelegate OnButtonEvent[4][3];
	FInputActionValue boolValue = FInputActionValue(true);

public:
	template <class UserClass>
	void BindAction(EUSButtonType BtnType, EUSBtnTouchTriggerEvent BtnTriggerEvent, UserClass* obj,
					FButtonEventDelegate::TMethodPtr<UserClass> Func)
	{
		OnButtonEvent[static_cast<int8>(BtnType)][static_cast<int8>(BtnTriggerEvent)].BindUObject(obj, Func);
	}

	void BindPlayer(AUSFightingCharacter* InControllingCharacter);
	void UnBindPlayer();

.
.
.
```

- 소스코드 링크: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/UI/USTouchInterfaceButton.h

## 3. 공통 상수/클래스

- 공통으로 사용되는 상수, 계속해서 사용되는String기반 상수등을 별도의 헤더파일, 네임스페이스에 `const static`으로 묶어 재사용성을 높이고 상수, String사용시의 실수를 방지

```cpp
.
.
.

namespace USSocketNames
{
	const static FName Shield = FName("shield_l");
	const static FName Weapon = FName("weapon_r");
	const static FName Weapon_Socket_slash_start = FName("Slash_Start");
	const static FName Weapon_Socket_slash_end = FName("Slash_End");
}

namespace USMaterialParamNames
{
	const static FName Tint=FName("Tint");
	const static FName Brightness=FName("Metal_Brightness");
}

namespace USTagNames
{
	const static FName Taken = FName("Taken");
	const static FName SpectatorStart = FName("SpectatorStart");
}

.
.
.
```

- 소스코드 링크: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/CharacterShare/EditorNames.h

## 4. 런타임 최적화


- 계속해서 사용되는 투사체의 경우 오브젝트 풀링을 활용하여 런타임 객체 할당/해제에 대한 부하를 완화
- 투사체를 던져 Pool으로부터 인스턴스를 World에 소환하는 역할 뿐 아니라 투사체가 적중하고 Pool에 돌아오는 역할을 델리게이트와 콜백함수로 제작하여 투사체의 라이프사이클을 스킬이 직접 관리하게 하여 클래스간 역할 분담을 명확히함

```cpp
class ANDROIDPROJECT_API AProjectileBase : public AActor
{
GENERATED_BODY()
public:
.
.
.
		virtual void DeactivateProjectile();
		virtual void ActivateProjectile(FVector SpawnPos, FVector CastDir, float Dmg, float Impact, float Speed);
.
.
.
		FOnProjCollideToTarget OnProjCollideToTarget; // Delegate
		FOnProjCollideWithNothing OnProjCollideWithNothing; // Delegate
};
```

```cpp
class ANDROIDPROJECT_API UThrowHandAxe : public USkillComponentBase
{
GENERATED_BODY()
.
.
.
private:

   void ReturnProjToPoolOnCollision(AProjectileBase* InProj, AUSFightingCharacter* Target = nullptr);
   void ReturnProjToPoolOnNotCollision(AProjectileBase* InProj);
	 // These functions connected to Delegate
};
```

- 소스코드 링크:
    - Projectile Base: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/BattleSystemBase/ProjectileBase.cpp
    - Throw Hand Axe: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Skill/ThrowHandAxe.h

  
---

- 한 번 캐스팅된 객체들은 멤버변수로 캐싱하여 활용하여 동적 캐스팅 부담을 줄임

```cpp
void AUSWeaponBase::SetOwner(AActor* NewOwner)
{
   Super::SetOwner(NewOwner);

      OwnerAUSCharacter = 
         NewOwner != nullptr ? CastChecked<AUSFightingCharacter>(NewOwner) : nullptr;
   
   for(auto skill : Skills)
   {
      skill->SetOwnerUSFighter(OwnerAUSCharacter);
      skill->SetComponentTickEnabled(true);
   }
}
```

```cpp
void USkillComponentBase::SetOwnerUSFighter(AUSFightingCharacter* InCharacter)
{

   if(!IsValid(InCharacter))
   {
      OwnerUSFighter = nullptr;
      OwnerWeapon = nullptr;
      return;
   }
   
   OwnerUSFighter = InCharacter;
   OwnerWeapon = InCharacter->GetCurEquippedWeapon();
}
```

- 소스코드 링크:
    - Weapon Base: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Weapons/USWeaponBase.h
    - Skill Base: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Skill/SkillComponentBase.h

## 5. 네트워크 최적화


- 플레이어의 상태를 비트마스크로 관리하여 플레이어 상태 리플리케이션 데이터 양을 줄임
- 상태가 비트마스크로 관리되기에 상태를 업데이트 할 때마다 상태 전반에 대한 고려를 강제함으로써 실수의 여지를 줄임

```cpp
UPROPERTY(Replicated, VisibleInstanceOnly,
   Meta = (Bitmask, BitmaskEnum = "EUSPlayerActionState"), Category = "Player State")
uint32 ActionStateBitMask = 0;
```

```cpp
UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
namespace EUSPlayerActionState
{
   enum State : int
   {
      None = 0 UMETA(Hidden),

      Move = 0x1,
      Jump = 0x1 << 1,
      Cast = 0x1 << 2,
      Impacted = 0x1 << 3,
      Blown = 0x1 << 4,
      Invincible = 0x1 << 5,
      Died = 0x1 << 6,

      Max = -1 UMETA(Hidden)
   };
}
```

- 소스코드 링크:
    - 비트마스크(USFightingCharacter.h): https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Characters/USFightingCharacter.h
    - 비트마스크 Enum(EUSPlayerActionState.h): https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/CharacterShare/EUSPlayerActionState.h

  
---

- 선입력 시스템을 활용하여 플레이어의 매 입력마다 RPC를 전송하는 것이 아닌 실제 플레이어가 해당 동작이 가능한 상황에서 최종 입력만 서버에 전송하여 RPC콜을 최소화

```cpp
/** Use Only Client*/
UPROPERTY(VisibleInstanceOnly)
FUSOrder BufferedOrder;

/** Use Only Server*/
UPROPERTY(VisibleInstanceOnly)
FUSOrder ServerBufferedOrder;
```

```cpp
if (BufferedOrder.IsMovementOrder() && IsOrderExecutableState()) // Process buffered order
{
   SendOrderToServer(BufferedOrder);
   BufferedOrder.Init();
}
```

- 소스코드 링크:
    - 플레이어 캐릭터 cpp: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Characters/USFightingCharacter.cpp
    - 플레이어 캐릭터 header: https://github.com/jellypower/USLanFighter/blob/master/Source/AndroidProject/Characters/USFightingCharacter.h

## 6. 편의기능


- 주위의 적을 자동으로 타겟 하는 오토 타게팅 시스템을 통한 조작감 개선
- 플레이어가 공격한 방향 기준으로 일정 각도 내에 적이 있다면 해당 타겟을 공격하도록 자동 에임 보정

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/AutoTargeting.gif" width="600"/>
</p>

  
---

- 플레이어 사망시에 다른 플레이어를 관전할 수 있는 관전 시스템

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/Spectating.gif" width="600"/>
</p>


## 7. 자체제작 매칭서버


- 경량화, 간소화하여 사용법이 간단한 매칭서버 제작
- 언리얼 엔진의 소켓 API를 통해 직접 매칭 서버 플러그인을 제작, 배포하여 플레이어가LAN에 열린 서버를 찾고 입장 가능
- 언리얼엔진의 Subsystem클래스를 활용하여 유저가 모듈만 import 한다면 손쉽게 서버를 열고 닫을 수 있도록 제작
- Git의 Submodule을 활용하여 분리된 프로젝트의 버전관리를 진행

<p align="center">
<img src="https://github.com/jellypower/PublicImageDataBase/blob/main/Portfolio/USLanFighter/MatchingServer.png" width="600"/>
</p>

- 프로젝트 링크: https://github.com/jellypower/USLanMatcher
