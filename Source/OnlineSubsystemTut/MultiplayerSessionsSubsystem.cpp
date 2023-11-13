// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	// PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// PrintString("MSS Initialize");

	if (const IOnlineSubsystem* OnlineSubsystem =  IOnlineSubsystem::Get())
	{
		const FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintString(SubsystemName);

		SessionInterface =  OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			PrintString("Session interface is valid");
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	// UE_LOG(LogTemp,Warning, TEXT("MSS Deinitialize"));
}

void UMultiplayerSessionsSubsystem::PrintString(const FString& Str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f,FColor::Cyan, Str);
	}
}
