// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceEmailFormCommands.h"

#define LOCTEXT_NAMESPACE "FEmergenceEmailFormModule"

void FEmergenceEmailFormCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "EmergenceEmailForm", "Bring up EmergenceEmailForm window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
