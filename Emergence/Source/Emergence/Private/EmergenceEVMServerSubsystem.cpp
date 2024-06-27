// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceEVMServerSubsystem.h"
#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "Runtime/Launch/Resources/Version.h"

void UEmergenceEVMServerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
#if UNREAL_MARKETPLACE_BUILD
	UHttpHelperLibrary::APIBase = "https://evm4.openmeta.xyz/api/";
#else
	bool LaunchHidden = true;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("LaunchServerHidden"), LaunchHidden, GGameIni);
	}
	ULocalEmergenceServer::LaunchLocalServerProcess(LaunchHidden);
#endif

#if (ENGINE_MINOR_VERSION >= 1) && (ENGINE_MAJOR_VERSION >= 5)
	bool AllowLibcurlConnectionReuse = false;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("AllowLibcurlConnectionReuse"), AllowLibcurlConnectionReuse, GGameIni);
		GConfig->SetBool(TEXT("HTTP.Curl"), TEXT("bForbidReuse"), !(AllowLibcurlConnectionReuse), GEngineIni);
		GConfig->Flush(false, GEngineIni);
	}
#endif

	FHttpModule::Get().SetMaxReadBufferSize(524288); //libcurl maximum allowed value
	FHttpModule::Get().SetHttpThreadActiveFrameTimeInSeconds(1.0f / 100000.f);
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