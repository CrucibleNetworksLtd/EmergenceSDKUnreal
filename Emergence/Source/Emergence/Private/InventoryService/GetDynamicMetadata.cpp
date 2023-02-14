// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/GetDynamicMetadata.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetDynamicMetadata* UGetDynamicMetadata::GetDynamicMetadata(UObject* WorldContextObject, const FString& Network, const FString& Contract, const FString& TokenID)
{
	UGetDynamicMetadata* BlueprintNode = NewObject<UGetDynamicMetadata>();
	BlueprintNode->Network = Network;
	BlueprintNode->Contract = Contract;
	BlueprintNode->TokenID = TokenID;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetDynamicMetadata::Activate()
{
	FString requestURL = UHttpHelperLibrary::GetInventoryServiceAPIURL() + "getItemMetadata" + "?network=" + Network + "&contract=" + Contract + "&tokenId=" + TokenID;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Host", UHttpHelperLibrary::GetInventoryServiceHostURL()});
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetDynamicMetadata>(
		this,
		&UGetDynamicMetadata::GetDynamicMetadata_HttpRequestComplete,
		requestURL);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetDynamicMetadata request started, calling GetDynamicMetadata_HttpRequestComplete on request completed"));
}

void UGetDynamicMetadata::GetDynamicMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnGetDynamicMetadataCompleted.Broadcast(JsonObject.GetStringField("message"), EErrorCode::EmergenceOk);
	}
	else {
		OnGetDynamicMetadataCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetDynamicMetadata", StatusCode);
	}
	SetReadyToDestroy();
}
