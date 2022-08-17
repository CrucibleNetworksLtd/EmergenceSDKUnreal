// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "EmergenceToolbarCommands.h"

#define LOCTEXT_NAMESPACE "FEmergenceToolbarModule"

void FEmergenceToolbarCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EmergenceToolbar", "Execute EmergenceToolbar action", EUserInterfaceActionType::Button, FInputChord());
	
	UI_COMMAND(StartServerAction, "Start server", "Start server", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StopServerAction, "Stop server", "Stop server", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(RestartServerAction, "Restart server", "Restart server", EUserInterfaceActionType::Button, FInputGesture());
	//UI_COMMAND(CheckServerStatusAction, "Check server status", "Check server status", EUserInterfaceActionType::Button, FInputGesture());

}

#undef LOCTEXT_NAMESPACE
