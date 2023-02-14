// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceEVMServerSubsystem.h"
#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"

void UEmergenceEVMServerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
#if UNREAL_MARKETPLACE_BUILD
	UHttpHelperLibrary::APIBase = "https://evm.openmeta.xyz/api/";
#else
	bool LaunchHidden = true;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("LaunchServerHidden"), LaunchHidden, GGameIni);
	}
	ULocalEmergenceServer::LaunchLocalServerProcess(LaunchHidden);
#endif
}

void UEmergenceEVMServerSubsystem::Deinitialize() {
#if !UNREAL_MARKETPLACE_BUILD
	ULocalEmergenceServer::KillLocalServerProcess();
#endif
}