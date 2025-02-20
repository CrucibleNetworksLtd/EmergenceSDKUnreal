// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "EmergenceCore.h"
#include "Environment.generated.h"

UENUM(BlueprintType)
enum class EEmergenceEnvironment : uint8
{
	Development UMETA(Hidden, ToolTip="For Crucible to test changes to Emergence before public release."),
	Staging UMETA(ToolTip="For testing functionality before you release your game. The staging environment functions identically to production."),
	Production UMETA(ToolTip="For when you release your game.")
};

UENUM(BlueprintType)
enum class EFutureverseEnvironment : uint8
{
	Development UMETA(ToolTip = "For testing with Porcini."),
	Staging UMETA(ToolTip = "For testing functionality before you release your game. The staging environment functions identically to production."),
	Production UMETA(ToolTip = "For when you release your game.")
};