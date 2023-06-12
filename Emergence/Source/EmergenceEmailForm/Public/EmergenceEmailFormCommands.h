// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EmergenceEmailFormStyle.h"

class FEmergenceEmailFormCommands : public TCommands<FEmergenceEmailFormCommands>
{
public:

	FEmergenceEmailFormCommands()
		: TCommands<FEmergenceEmailFormCommands>(TEXT("EmergenceEmailForm"), NSLOCTEXT("Contexts", "EmergenceEmailForm", "EmergenceEmailForm Plugin"), NAME_None, FEmergenceEmailFormStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};