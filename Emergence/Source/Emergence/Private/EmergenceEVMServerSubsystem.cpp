// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "EmergenceEVMServerSubsystem.h"
#include "LocalEmergenceServer.h"

void UEmergenceEVMServerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	bool LaunchHidden = true;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("LaunchServerHidden"), LaunchHidden, GGameIni);
	}
	ULocalEmergenceServer::LaunchLocalServerProcess(LaunchHidden);
}

void UEmergenceEVMServerSubsystem::Deinitialize() {
	ULocalEmergenceServer::KillLocalServerProcess();
}