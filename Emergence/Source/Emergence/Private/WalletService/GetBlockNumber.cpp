// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/GetBlockNumber.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChain.h"

UGetBlockNumber* UGetBlockNumber::GetBlockNumber(const UObject* WorldContextObject, FString NodeURL)
{
	UGetBlockNumber* BlueprintNode = NewObject<UGetBlockNumber>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->NodeURL = NodeURL;
	return BlueprintNode;
}

void UGetBlockNumber::Activate()
{
	if (NodeURL.IsEmpty()) {
		NodeURL = UChainDataLibrary::GetEmergenceChainDataFromConfig().GetChainURL();
	}
	UHttpHelperLibrary::ExecuteHttpRequest<UGetBlockNumber>(
		this, 
		&UGetBlockNumber::GetBlockNumber_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "getBlockNumber?nodeURL=" + NodeURL);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetBlockNumber request started with JSON, calling GetBlockNumber_HttpRequestComplete on request completed."));
}

void UGetBlockNumber::GetBlockNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {	
		OnGetBlockNumberCompleted.Broadcast(JsonObject.GetObjectField("message")->GetIntegerField("blockNumber"), EErrorCode::EmergenceOk);
		return;
	}
	OnGetBlockNumberCompleted.Broadcast(-1, StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetBlockNumber", StatusCode);
}
