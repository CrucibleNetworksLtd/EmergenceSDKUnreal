// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/WriteDynamicMetadata.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UWriteDynamicMetadata* UWriteDynamicMetadata::WriteDynamicMetadata(UObject* WorldContextObject, const FString& Network, const FString& AuthorizationHeader, const FString& Contract, const FString& TokenID, const FString& Metadata/*, const bool OnlyUpdate*/)
{
	UWriteDynamicMetadata* BlueprintNode = NewObject<UWriteDynamicMetadata>();
	BlueprintNode->AuthorizationHeader = AuthorizationHeader;
	BlueprintNode->Network = Network;
	BlueprintNode->Contract = Contract;
	BlueprintNode->TokenID = TokenID;
	BlueprintNode->Metadata = Metadata;
	BlueprintNode->OnlyUpdate = true/*OnlyUpdate*/;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UWriteDynamicMetadata::Activate()
{
	FString Endpoint = OnlyUpdate ? "updateMetadata" : "putMetadata";
	FString Method = OnlyUpdate ? "POST" : "PUT";
	FString requestURL = UHttpHelperLibrary::GetInventoryServiceAPIURL() + Endpoint + "?network=" + Network + "&contract=" + Contract + "&tokenId=" + TokenID;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization-header", AuthorizationHeader});
	Headers.Add(TPair<FString, FString>{"Host", UHttpHelperLibrary::GetInventoryServiceHostURL()});
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	UHttpHelperLibrary::ExecuteHttpRequest<UWriteDynamicMetadata>(
		this,
		&UWriteDynamicMetadata::WriteDynamicMetadata_HttpRequestComplete,
		requestURL,
		Method,
		60.0F,
		Headers, 
		"{\"metadata\": " + Metadata + "}"
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteDynamicMetadata request started, calling WriteDynamicMetadata_HttpRequestComplete on request completed"));
}

void UWriteDynamicMetadata::WriteDynamicMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnWriteDynamicMetadataCompleted.Broadcast(JsonObject.GetStringField("message"), EErrorCode::EmergenceOk);
	}
	else {
		OnWriteDynamicMetadataCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("WriteDynamicMetadata", StatusCode);
	}
	SetReadyToDestroy();
}
