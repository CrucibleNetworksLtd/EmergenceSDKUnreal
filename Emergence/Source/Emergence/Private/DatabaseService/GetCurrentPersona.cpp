// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "DatabaseService/GetCurrentPersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetCurrentPersona* UGetCurrentPersona::GetCurrentPersona(UObject* WorldContextObject)
{
	UGetCurrentPersona* BlueprintNode = NewObject<UGetCurrentPersona>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetCurrentPersona::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetCurrentPersona>(
		this, 
		&UGetCurrentPersona::GetCurrentPersona_HttpRequestComplete, 
		UHttpHelperLibrary::GetPersonaAPIURL() + "persona",
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetCurrentPersona request started, calling GetCurrentPersona_HttpRequestComplete on request completed"));
}

void UGetCurrentPersona::GetCurrentPersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnGetCurrentPersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);

		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->SetCachedCurrentPersona(ResponceStruct);
	}
	else {
		OnGetCurrentPersonaCompleted.Broadcast(FEmergencePersona(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetCurrentPersonaById", StatusCode);
	}
	SetReadyToDestroy();
}
