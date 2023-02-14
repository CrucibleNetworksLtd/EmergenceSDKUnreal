// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "DatabaseService/UpdatePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UUpdatePersona* UUpdatePersona::UpdatePersona(UObject* WorldContextObject, FEmergencePersona Persona)
{
	UUpdatePersona* BlueprintNode = NewObject<UUpdatePersona>();
	BlueprintNode->Persona = Persona;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UUpdatePersona::Activate()
{
	FString PersonaJsonString;
	FJsonObjectConverter::UStructToJsonObjectString<FEmergencePersona>(this->Persona, PersonaJsonString);
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});
	UHttpHelperLibrary::ExecuteHttpRequest<UUpdatePersona>(
		this,
		&UUpdatePersona::UpdatePersona_HttpRequestComplete,
		UHttpHelperLibrary::GetPersonaAPIURL() + "persona",
		"PATCH",
		60.0F,
		Headers,
		PersonaJsonString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("UpdatePersona request started with JSON, calling UpdatePersona_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *PersonaJsonString);
}

void UUpdatePersona::UpdatePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnUpdatePersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
	}
	else {
		OnUpdatePersonaCompleted.Broadcast(this->Persona, StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("UpdatePersona", StatusCode);
	}
	SetReadyToDestroy();
}
