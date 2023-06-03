// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LoginManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANDROIDPROJECT_API ULoginManagerComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Unreal Events
public:	
	ULoginManagerComponent();

protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region LoginManager Setting



#pragma endregion 

		
};
