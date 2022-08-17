// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/GetTransactionStatus.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChain.h"

UGetTransactionStatus* UGetTransactionStatus::GetTransactionStatus(const UObject* WorldContextObject, FString TransactionHash, FString NodeURL)
{
	UGetTransactionStatus* BlueprintNode = NewObject<UGetTransactionStatus>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TransactionHash = TransactionHash;
	BlueprintNode->NodeURL = NodeURL;
	return BlueprintNode;
}

void UGetTransactionStatus::Activate()
{
	if (NodeURL.IsEmpty()) {
		NodeURL = UChainDataLibrary::GetEmergenceChainDataFromConfig().GetChainURL();
	}
	UHttpHelperLibrary::ExecuteHttpRequest<UGetTransactionStatus>(
		this, 
		&UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "GetTransactionStatus?transactionHash=" + TransactionHash + "&nodeURL=" + NodeURL);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus request started with JSON, calling GetTransactionStatus_HttpRequestComplete on request completed."));
}

void UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {	
		FString TransactionAsJSONString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&TransactionAsJSONString);
		FJsonSerializer::Serialize(JsonObject.GetObjectField("message")->GetObjectField("transaction").ToSharedRef(), Writer);
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(TransactionAsJSONString), EErrorCode::EmergenceOk);
		return;
	}
	OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTransactionStatus", StatusCode);
}
