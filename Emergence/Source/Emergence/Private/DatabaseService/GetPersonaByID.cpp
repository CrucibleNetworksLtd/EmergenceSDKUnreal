// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "DatabaseService/GetPersonaByID.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetPersonaByID* UGetPersonaByID::GetPersonaByID(UObject* WorldContextObject, const FString& personaID)
{
	UGetPersonaByID* BlueprintNode = NewObject<UGetPersonaByID>();
	BlueprintNode->PersonaID = FString(personaID);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetPersonaByID::Activate()
{
	FString requestURL = UHttpHelperLibrary::GetPersonaAPIURL() + "persona/" + PersonaID;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetPersonaByID>(
		this,
		&UGetPersonaByID::GetPersonaByID_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetPersonaByID request started, calling GetPersonaByID_HttpRequestComplete on request completed"));
}

void UGetPersonaByID::GetPersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnGetPersonaByIDCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
	}
	else {
		OnGetPersonaByIDCompleted.Broadcast(FEmergencePersona(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetPersonaById", StatusCode);
	}
	SetReadyToDestroy();
}
