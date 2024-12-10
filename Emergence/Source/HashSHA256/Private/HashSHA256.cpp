// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#include "HashSHA256.h"

#define LOCTEXT_NAMESPACE "FHashSHA256Module"

void FHashSHA256Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FHashSHA256Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHashSHA256Module, HashSHA256)