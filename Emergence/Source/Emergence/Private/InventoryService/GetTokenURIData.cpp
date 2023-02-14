// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/GetTokenURIData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetTokenURIData* UGetTokenURIData::GetTokenURIData(UObject* WorldContextObject, const FString& TokenURI)
{
	UGetTokenURIData* BlueprintNode = NewObject<UGetTokenURIData>();
	BlueprintNode->TokenURI = FString(TokenURI);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetTokenURIData::Activate()
{
	FString requestURL = TokenURI;
	TArray<TPair<FString, FString>> Headers;

	UHttpHelperLibrary::ExecuteHttpRequest<UGetTokenURIData>(
		this,
		&UGetTokenURIData::GetTokenURIData_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTokenURIData request started, calling GetTokenURIData_HttpRequestComplete on request completed"));
}

void UGetTokenURIData::GetTokenURIData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	StatusCode = EErrorCode::EmergenceOk; //FORCE IT, ONLY FOR TESTING
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnGetTokenURIDataCompleted.Broadcast(*HttpResponse->GetContentAsString(), EErrorCode::EmergenceOk);
	}
	else {
		OnGetTokenURIDataCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTokenURIData", StatusCode);
	}
	SetReadyToDestroy();
}
