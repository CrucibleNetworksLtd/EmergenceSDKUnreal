// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "DatabaseService/DeletePersonaByID.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UDeletePersonaByID* UDeletePersonaByID::DeletePersonaByID(UObject* WorldContextObject, const FString& PersonaID)
{
	UDeletePersonaByID* BlueprintNode = NewObject<UDeletePersonaByID>();
	BlueprintNode->PersonaID = FString(PersonaID);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UDeletePersonaByID::Activate()
{
	FString requestURL = UHttpHelperLibrary::GetPersonaAPIURL() + "persona/" + PersonaID;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});
	UHttpHelperLibrary::ExecuteHttpRequest<UDeletePersonaByID>(
		this, 
		&UDeletePersonaByID::DeletePersonaByID_HttpRequestComplete, 
		requestURL,
		"DELETE",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("DeletePersonaByID request started (request URL '%s'), calling DeletePersonaByID_HttpRequestComplete on request completed"), *requestURL);
}

void UDeletePersonaByID::DeletePersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("Response: %s"), *HttpResponse->GetContentAsString());
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnDeletePersonaByIDCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
	}
	else {
		OnDeletePersonaByIDCompleted.Broadcast(FEmergencePersona(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("DeletePersonaById", StatusCode);
	}
	SetReadyToDestroy();
}
