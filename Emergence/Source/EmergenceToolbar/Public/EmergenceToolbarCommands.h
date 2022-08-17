// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EmergenceToolbarStyle.h"

class FEmergenceToolbarCommands : public TCommands<FEmergenceToolbarCommands>
{
public:

	FEmergenceToolbarCommands()
		: TCommands<FEmergenceToolbarCommands>(TEXT("EmergenceToolbar"), NSLOCTEXT("Contexts", "EmergenceToolbar", "EmergenceToolbar Plugin"), NAME_None, FEmergenceToolbarStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
	TSharedPtr< FUICommandInfo > CheckServerStatusAction;
	TSharedPtr< FUICommandInfo > StartServerAction;
	TSharedPtr< FUICommandInfo > StopServerAction;
	TSharedPtr< FUICommandInfo > RestartServerAction;

};
