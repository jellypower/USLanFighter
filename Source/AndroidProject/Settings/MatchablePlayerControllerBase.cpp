// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/MatchablePlayerControllerBase.h"

#include "USLANMatcherHostServerSubsystem.h"

void AMatchablePlayerControllerBase::BeginPlay()
{
	Super::PostInitializeComponents();
	if(GetGameInstance() != nullptr)
		ServerSubsystem = GetGameInstance()->GetSubsystem<UUSLANMatcherHostServerSubsystem>();
}

bool AMatchablePlayerControllerBase::HostMatchingServer()
{
	return ServerSubsystem->HostServer() == SOCKET_CONSTRUCT_RESULT::SUCCEDED;
}


bool AMatchablePlayerControllerBase::IsServerHosted()
{
	return ServerSubsystem->GetbIsServerHosting() != 0;
}

void AMatchablePlayerControllerBase::CloseMatchingServer()
{
	ServerSubsystem->CloseServer();
}

FString AMatchablePlayerControllerBase::GetCurrentWorldIP(bool bWithPortNo) const
{
	return ServerSubsystem->GetCurrentWorldIP();
}

int32 AMatchablePlayerControllerBase::GetMatchingServerPortNO()
{
	return ServerSubsystem->GetPortNo();
	
}

FString AMatchablePlayerControllerBase::GetMatchingServerID()
{
	return ServerSubsystem->GetServerID();
}
