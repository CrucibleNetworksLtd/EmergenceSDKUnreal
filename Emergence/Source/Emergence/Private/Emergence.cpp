// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Emergence.h"
DEFINE_LOG_CATEGORY(LogEmergence)
DEFINE_LOG_CATEGORY(LogEmergenceHttp)

#define LOCTEXT_NAMESPACE "FEmergenceModule"

void FEmergenceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEmergenceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceModule, Emergence)