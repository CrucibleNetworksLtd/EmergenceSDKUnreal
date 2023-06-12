// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceEVMServerSubsystem.h"
#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "HttpModule.h"
#include "HttpManager.h"

void UEmergenceEVMServerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
#if UNREAL_MARKETPLACE_BUILD
	UHttpHelperLibrary::APIBase = "https://evm2.openmeta.xyz/api/";
#else
	bool LaunchHidden = true;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("LaunchServerHidden"), LaunchHidden, GGameIni);
	}
	ULocalEmergenceServer::LaunchLocalServerProcess(LaunchHidden);
#endif
}

void UEmergenceEVMServerSubsystem::Deinitialize() {

	for (FHttpRequestRef Request : ActiveRequests) {
		Request->OnProcessRequestComplete().Unbind();
		Request->CancelRequest();		
	}

#if !UNREAL_MARKETPLACE_BUILD
	ULocalEmergenceServer::KillLocalServerProcess();
#else
	UEmergenceSingleton::GetEmergenceManager(GetGameInstance())->KillSession();
#endif
}