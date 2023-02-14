// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "DatabaseService/SetActivePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

USetActivePersona* USetActivePersona::SetActivePersona(UObject* WorldContextObject, const FString& PersonaID)
{
	USetActivePersona* BlueprintNode = NewObject<USetActivePersona>();
	BlueprintNode->PersonaID = PersonaID;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void USetActivePersona::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<USetActivePersona>(
		this, 
		&USetActivePersona::SetActivePersona_HttpRequestComplete,
		UHttpHelperLibrary::GetPersonaAPIURL() + "setActivePersona/" + this->PersonaID,
		"PATCH",
		60.0F,
		Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("SetActivePersona request started, calling SetActivePersona_HttpRequestComplete on request completed"));
}

void USetActivePersona::SetActivePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnSetActivePersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
	}
	else {
		OnSetActivePersonaCompleted.Broadcast(FEmergencePersona(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("SetActivePersona", StatusCode);
	}
	SetReadyToDestroy();
}
