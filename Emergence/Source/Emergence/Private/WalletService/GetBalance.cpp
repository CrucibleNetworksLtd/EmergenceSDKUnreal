// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/GetBalance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChain.h"

UGetBalance* UGetBalance::GetBalance(const UObject* WorldContextObject, FString Address)
{
	UGetBalance* BlueprintNode = NewObject<UGetBalance>();
	BlueprintNode->Address = Address;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetBalance::Activate()
{
	FEmergenceChainStruct ChainData = UChainDataLibrary::GetEmergenceChainDataFromConfig();

	FString NodeURL = ChainData.GetChainURL();
	UE_LOG(LogEmergenceHttp, Warning, TEXT("Using Node URL: %s"), *NodeURL);

	UHttpHelperLibrary::ExecuteHttpRequest<UGetBalance>(
		this, 
		&UGetBalance::GetBalance_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "getbalance" + "?nodeUrl=" + NodeURL + "&address=" + this->Address);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetBalance request started with JSON, calling GetBalance_HttpRequestComplete on request completed. Json sent as part of the request: "));
}

void UGetBalance::GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Balance;
		if (JsonObject.GetObjectField("message")->TryGetStringField("balance", Balance)) {
			OnGetBalanceCompleted.Broadcast(Balance, StatusCode);
		}
		else {
			OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetBalanceCompleted.Broadcast(FString(), StatusCode);
}
