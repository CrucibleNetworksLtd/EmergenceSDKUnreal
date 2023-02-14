// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetBalance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"

UGetBalance* UGetBalance::GetBalance(UObject* WorldContextObject, FString Address, UEmergenceChain* Blockchain)
{
	UGetBalance* BlueprintNode = NewObject<UGetBalance>();
	BlueprintNode->Address = Address;
	BlueprintNode->Blockchain = Blockchain;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetBalance::Activate()
{
	if (Blockchain) {
		UHttpHelperLibrary::ExecuteHttpRequest<UGetBalance>(
			this,
			&UGetBalance::GetBalance_HttpRequestComplete,
			UHttpHelperLibrary::APIBase + "getbalance" + "?nodeUrl=" + Blockchain->NodeURL + "&address=" + this->Address);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetBalance request started with JSON, calling GetBalance_HttpRequestComplete on request completed. Json sent as part of the request: "));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("GetBalance's blockchain input was null."));
		OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
	}
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
	}
	else {
		OnGetBalanceCompleted.Broadcast(FString(), StatusCode);
	}
	SetReadyToDestroy();
}
