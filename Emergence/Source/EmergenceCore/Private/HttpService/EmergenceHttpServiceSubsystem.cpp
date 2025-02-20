// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "HttpService/EmergenceHttpServiceSubsystem.h"
#include "HttpService/HttpHelperLibrary.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "Runtime/Launch/Resources/Version.h"

void UEmergenceHttpServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection) {

	//@TODO now the LocalEVM server is no more, we should probably move this code somewhere else and get rid of this class

	//This allows the user to configure if they want to allow conection reuse via our plugin settings, 
	//as these are not usually configurable in the UI by default
#if (ENGINE_MINOR_VERSION >= 1) && (ENGINE_MAJOR_VERSION >= 5)
	bool AllowLibcurlConnectionReuse = false;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("AllowLibcurlConnectionReuse"), AllowLibcurlConnectionReuse, GGameIni);
		GConfig->SetBool(TEXT("HTTP.Curl"), TEXT("bForbidReuse"), !(AllowLibcurlConnectionReuse), GEngineIni);
		GConfig->Flush(false, GEngineIni);
	}
#endif

	//These two adjustments below allow faster download of large files. 
	//Remove the lines or adjust the values if you get hitching in your game when downloading files from the internet.
	FHttpModule::Get().SetMaxReadBufferSize(524288); //libcurl maximum allowed value
	FHttpModule::Get().SetHttpThreadActiveFrameTimeInSeconds(1.0f / 100000.f);
}

void UEmergenceHttpServiceSubsystem::Deinitialize() {
	
	//clean up all in-flight requests
	for (FHttpRequestRef Request : ActiveRequests) {
		Request->OnProcessRequestComplete().Unbind();
		Request->CancelRequest();		
	}
}