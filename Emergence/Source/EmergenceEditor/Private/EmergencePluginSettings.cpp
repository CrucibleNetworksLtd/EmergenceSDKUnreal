// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergencePluginSettings.h"
#include "Runtime/Launch/Resources/Version.h"

UEmergencePluginSettings::UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    EngineHasConnectionReuseConfig = (ENGINE_MINOR_VERSION >= 1) && (ENGINE_MAJOR_VERSION >= 5);
}
