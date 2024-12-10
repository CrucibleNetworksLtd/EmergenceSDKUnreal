// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FHashSHA256Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
