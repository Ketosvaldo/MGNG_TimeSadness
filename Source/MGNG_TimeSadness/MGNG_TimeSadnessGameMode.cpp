// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGNG_TimeSadnessGameMode.h"
#include "MGNG_TimeSadnessCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMGNG_TimeSadnessGameMode::AMGNG_TimeSadnessGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
