// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemTutGameMode.h"
#include "OnlineSubsystemTutCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOnlineSubsystemTutGameMode::AOnlineSubsystemTutGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
