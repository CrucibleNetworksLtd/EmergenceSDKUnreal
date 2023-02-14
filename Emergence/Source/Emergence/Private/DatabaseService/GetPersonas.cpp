// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "DatabaseService/GetPersonas.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetPersonas* UGetPersonas::GetPersonas(UObject* WorldContextObject)
{
	UGetPersonas* BlueprintNode = NewObject<UGetPersonas>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetPersonas::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetPersonas>(
		this,
		&UGetPersonas::GetPersonas_HttpRequestComplete,
		UHttpHelperLibrary::GetPersonaAPIURL() + "personas",
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetPersonas request started, calling GetPersonas_HttpRequestComplete on request completed"));
}

void UGetPersonas::GetPersonas_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersonaListResponse ResponceStruct = FEmergencePersonaListResponse(*HttpResponse->GetContentAsString());
		OnGetPersonasCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		
		for (int i = 0; i < ResponceStruct.personas.Num(); i++) {
			if (ResponceStruct.personas[i].id == ResponceStruct.selected) {
				UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->SetCachedCurrentPersona(ResponceStruct.personas[i]);
			}
		}
	}
	else {
		OnGetPersonasCompleted.Broadcast(FEmergencePersonaListResponse(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetPersonas", StatusCode);
	}
	SetReadyToDestroy();
}
