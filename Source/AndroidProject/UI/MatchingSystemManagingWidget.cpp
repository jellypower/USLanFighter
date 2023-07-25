// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MatchingSystemManagingWidget.h"

#include "USLANMatcherHostServerSubsystem.h"

void UMatchingSystemManagingWidget::NativeConstruct()
{
	ServerSubsystem = GetGameInstance()->GetSubsystem<UUSLANMatcherHostServerSubsystem>();
	Super::NativeConstruct();
}

bool UMatchingSystemManagingWidget::HostMatchingServer()
{
	return ServerSubsystem->HostServer() == SOCKET_CONSTRUCT_RESULT::SUCCEDED;
}

bool UMatchingSystemManagingWidget::IsServerHosted()
{
	return ServerSubsystem->GetbIsServerHosting() != 0;
}

void UMatchingSystemManagingWidget::ProcessJoinRequests()
{
	ServerSubsystem->ProcessRequests(ServerSubsystem->GetCurrentWorldIP());
}

void UMatchingSystemManagingWidget::CloseMatchingServer()
{
	ServerSubsystem->CloseServer();
}


FString UMatchingSystemManagingWidget::GetCurrentWorldIP(bool bWithPortNo) const
{
	return ServerSubsystem->GetCurrentWorldIP(bWithPortNo);
}

int32 UMatchingSystemManagingWidget::GetMatchingServerPortNO() const
{
	return ServerSubsystem->GetPortNo();
}

FString UMatchingSystemManagingWidget::GetMatchingServerID() const
{
	return ServerSubsystem->GetServerID();
}

void UMatchingSystemManagingWidget::TravelToBattleField(FString InWorldName)
{
	GetWorld()->ServerTravel(InWorldName);
}
