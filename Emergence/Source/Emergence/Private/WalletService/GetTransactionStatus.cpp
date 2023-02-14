// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetTransactionStatus.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"

UGetTransactionStatus* UGetTransactionStatus::GetTransactionStatus(UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* Blockchain)
{
	UGetTransactionStatus* BlueprintNode = NewObject<UGetTransactionStatus>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TransactionHash = TransactionHash;
	BlueprintNode->Blockchain = Blockchain;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetTransactionStatus::Activate()
{
	if (Blockchain) {
		UHttpHelperLibrary::ExecuteHttpRequest<UGetTransactionStatus>(
			this,
			&UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete,
			UHttpHelperLibrary::APIBase + "GetTransactionStatus?transactionHash=" + TransactionHash + "&nodeURL=" + Blockchain->NodeURL);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus request started with JSON, calling GetTransactionStatus_HttpRequestComplete on request completed."));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("GetTransactionStatus' blockchain input was null."));
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceClientFailed);
	}
}

void UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {	
		FString TransactionAsJSONString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&TransactionAsJSONString);
		if (!JsonObject.HasField("message") || JsonObject.GetObjectField("message")->HasTypedField<EJson::Null>("transaction")) {
			OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceClientJsonParseFailed);
		}
		else {
			FJsonSerializer::Serialize(JsonObject.GetObjectField("message")->GetObjectField("transaction").ToSharedRef(), Writer);
			OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(TransactionAsJSONString), EErrorCode::EmergenceOk);
		}
	}
	else {
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTransactionStatus", StatusCode);
	}
	SetReadyToDestroy();
}
