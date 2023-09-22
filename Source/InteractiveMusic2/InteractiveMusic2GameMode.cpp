// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractiveMusic2GameMode.h"
#include "InteractiveMusic2Character.h"
#include "UObject/ConstructorHelpers.h"

AInteractiveMusic2GameMode::AInteractiveMusic2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
