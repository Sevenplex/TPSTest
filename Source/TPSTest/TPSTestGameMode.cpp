// Copyright Maxim Dudin, 2024. All Rights Reserved.

#include "TPSTestGameMode.h"
#include "TPSTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPSTestGameMode::ATPSTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
