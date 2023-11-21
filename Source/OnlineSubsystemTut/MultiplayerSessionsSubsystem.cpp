// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	// PrintString("MSS Constructor");

	CreateServerAfterDestroy = false;
	DestroyServerName = "";
	ServerNameToFind = "";
	MySessionName = FName("Co-op Adventure Session Name");
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
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionsComplete);
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

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
	PrintString("Create Server");

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty");
		ServerCreateDel.Broadcast(false);
		return;
	}
	
	if (FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName))
	{
		FString Msg = FString::Printf(TEXT("Session with name %s already exists, destorying it..."), *MySessionName.ToString());
		PrintString(Msg);
		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName); // this destroys the session if it already exists
		return; // we don't continue with the rest of the function, as DestroySession is asynchronous 
	}

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true; // should other players be able to find the match?
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSettings.bIsLANMatch = IsLAN; // when using onlinesubsystemNull = true, if using subsystemsteam = false

	// this stores a server name into the session settings
	SessionSettings.Set(FName("Server_Name"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// this function doesn't complete until OnCreateSessionComplete delegate is called
	SessionInterface->CreateSession(0,MySessionName, SessionSettings); 
	
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
	PrintString("Find Server");

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty");
		ServerJoinDel.Broadcast(false);
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch()); // make shareable so it becomes TSharePtr
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	// prints 1 if successful, 0 if not
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

	ServerCreateDel.Broadcast(WasSuccessful);
	
	if (WasSuccessful)
	{
		FString Path = "/Game/ThirdPerson/Maps/ThirdPersonMap?listen";
		// note we've appended ?listen to launch it as a listen server
		if (!GameMapPath.IsEmpty())
		{
			Path = FString::Printf(TEXT("%s?listen"), *GameMapPath);
		}
		GetWorld()->ServerTravel(Path);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	// prints 1 if successful, 0 if not
	PrintString(FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Success: %d"),*SessionName.ToString(), WasSuccessful));
	
	if (CreateServerAfterDestroy)
	{
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
	if (!WasSuccessful) return;
	if (ServerNameToFind.IsEmpty()) return;
	
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* CorrectResult = 0;
	
	if (Results.Num()>0)
	{
		FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
		PrintString(Msg);

		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-name";
				Result.Session.SessionSettings.Get(FName("Server_Name"), ServerName);
				
				if (ServerName.Equals(ServerNameToFind))
				{
					CorrectResult = &Result;
					FString Msg2 = FString::Printf(TEXT("Found server with name: %s"), *ServerName);
					PrintString(Msg2);
					break; // stop for loop, as found the correct result
				}
			}
		}
		if (CorrectResult)
		{
			// this is async, so JoinSessionComplete is called after finished
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
		}
		else
		{
			PrintString(FString::Printf(TEXT("Couldn't find server: %s"), *ServerNameToFind));
			ServerNameToFind = "";
			ServerJoinDel.Broadcast(false);
		}
	}
	else
	{
		PrintString("Zero sessions found");
		ServerJoinDel.Broadcast(false);
	}
}


void UMultiplayerSessionsSubsystem::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
	
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Msg = FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString());
		PrintString(Msg);

		FString Address = "";
		// if getresolvedconnectstring is successful, we store the address in the Address variable
		if (bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address))
		{
			PrintString(FString::Printf(TEXT("Address: %s"), *Address));
			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
			
		}
		else
		{
			PrintString("GetResolvedConnectString returned false");
		}
	}
	else
	{
		PrintString("OnJoinSessionComplete failed");
	}
}
