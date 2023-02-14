// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "DatabaseService/CreatePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreatePersona* UCreatePersona::CreatePersona(UObject* WorldContextObject, FEmergencePersona Persona)
{
	UCreatePersona* BlueprintNode = NewObject<UCreatePersona>();
	BlueprintNode->TempPersona = Persona;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCreatePersona::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	FString PersonaJsonString;
	FJsonObjectConverter::UStructToJsonObjectString<FEmergencePersona>(this->TempPersona, PersonaJsonString);
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreatePersona>(
		this, 
		&UCreatePersona::CreatePersona_HttpRequestComplete, 
		UHttpHelperLibrary::GetPersonaAPIURL() + "persona",
		"POST",
		60.0F,
		Headers,
		PersonaJsonString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("CreatePersona request started with JSON, calling CreatePersona_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *PersonaJsonString);
}

void UCreatePersona::CreatePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnCreatePersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
	}
	else {
		OnCreatePersonaCompleted.Broadcast(FEmergencePersona(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("CreatePersona", StatusCode);
	}
	SetReadyToDestroy();
}
