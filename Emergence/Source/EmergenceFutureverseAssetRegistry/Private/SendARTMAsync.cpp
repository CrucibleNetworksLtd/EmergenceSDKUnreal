// Copyright Crucible Networks Ltd 2024. All Rights Reserved.


#include "SendARTMAsync.h"
#include "WalletService/RequestToSign.h"
#include "UObject/UObjectGlobals.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "TimerManager.h"
#include "EmergenceSingleton.h"
#include "EmergenceFutureverseAssetRegistry.h"

USendFutureverseARTM* USendFutureverseARTM::SendFutureverseARTM(UObject* _WorldContextObject, FString Message, TArray<FFutureverseARTMOperation> ARTMOperations) {
	USendFutureverseARTM* BlueprintNode = NewObject<USendFutureverseARTM>();
	BlueprintNode->_MessageToUser = Message;
	BlueprintNode->_ARTMOperations = ARTMOperations;
	BlueprintNode->WorldContextObject = _WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(_WorldContextObject);
	return BlueprintNode;
}

void USendFutureverseARTM::Activate()
{
	if (!WorldContextObject) {
		UE_LOG(LogEmergence, Error, TEXT("No world context object."));
		return;
	}

	FString EoAAddress = UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCachedAddress(true);
	if (EoAAddress.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("Tried to get the user EOA address but it has never been set."));
		return;
	}

	//THIS MUST BE EOA / ETH CHECKSUMMED
	GetNonceRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendFutureverseARTM>(
		this,
		nullptr,
		FEmergenceFutureverseAssetRegistryModule::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		TArray<TPair<FString,FString>>(),
		FString(), false);
	GetNonceRequest->SetHeader("content-type", "application/json");
	GetNonceRequest->SetContentAsString(R"({"query":"query GetNonce($input: NonceInput!) {\n  getNonceForChainAddress(input: $input)\n}","variables":{"input":{"chainAddress":")" + EoAAddress + "\"}}}");
	GetNonceRequest->OnProcessRequestComplete().BindLambda([&, EoAAddress](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		//when the request finishes
		EErrorCode StatusCode;
		FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(req, res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetNonce_HttpRequestComplete: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk && !JsonObject.HasField("errors")) {
			int Nonce = JsonObject.GetObjectField("data")->GetIntegerField("getNonceForChainAddress");
			ConstructedMessage = UARTMBuilderLibrary::GenerateARTM(_MessageToUser, _ARTMOperations, EoAAddress, FString::FromInt(Nonce)).ReplaceCharWithEscapedChar();
			URequestToSign* URequestToSign = URequestToSign::RequestToSign(WorldContextObject, ConstructedMessage);
			UE_LOG(LogEmergenceHttp, Display, TEXT("Sending the following message to Request to Sign: %s"), *ConstructedMessage);
			URequestToSign->OnRequestToSignCompleted.AddDynamic(this, &USendFutureverseARTM::OnRequestToSignCompleted);
			RequestToSignRequest = URequestToSign->Request;
			URequestToSign->Activate();
			return;
		}
		else {
			//handle fail to parse errors
			OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			UE_LOG(LogEmergence, Error, TEXT("Send Mutation Request failed"));
			return;
		}
	});
	GetNonceRequest->ProcessRequest();
	
}

void USendFutureverseARTM::Cancel()
{
	if (GetNonceRequest) {
		GetNonceRequest->OnProcessRequestComplete().Unbind();
		GetNonceRequest->CancelRequest();
	}
	if (RequestToSignRequest) {
		RequestToSignRequest->OnProcessRequestComplete().Unbind();
		RequestToSignRequest->CancelRequest();
	}
	if (SendMutationRequest) {
		SendMutationRequest->OnProcessRequestComplete().Unbind();
		SendMutationRequest->CancelRequest();
	}
	if (GetTransactionStatusRequest) {
		GetTransactionStatusRequest->OnProcessRequestComplete().Unbind();
		GetTransactionStatusRequest->CancelRequest();
	}
}

bool USendFutureverseARTM::IsActive() const
{
	return GetNonceRequest->GetStatus() == EHttpRequestStatus::Processing
		|| RequestToSignRequest->GetStatus() == EHttpRequestStatus::Processing
		|| SendMutationRequest->GetStatus() == EHttpRequestStatus::Processing
		|| GetTransactionStatusRequest->GetStatus() == EHttpRequestStatus::Processing;
}

void USendFutureverseARTM::OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode)
{
	UE_LOG(LogEmergenceHttp, Display, TEXT("OnRequestToSignCompleted: %s"), *SignedMessage);

	if (StatusCode != EErrorCode::EmergenceOk) {
		//handle fail to parse errors
		OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		UE_LOG(LogEmergence, Error, TEXT("Request To Sign Request failed"));
		return;
	}

	SendMutationRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendFutureverseARTM>(
		this,
		nullptr,
		FEmergenceFutureverseAssetRegistryModule::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		60.0F,
		TArray<TPair<FString, FString>>(),
		FString(), false);
	SendMutationRequest->SetHeader("content-type", "application/json");
	FString JSONString = R"({"query":"mutation SubmitTransaction($input: SubmitTransactionInput!) {\n  submitTransaction(input: $input) {\n    transactionHash\n  }\n}","variables":{"input":{"transaction":")" + ConstructedMessage + R"(","signature":")" + SignedMessage + "\"}}}";
	SendMutationRequest->SetContentAsString(JSONString);
	SendMutationRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		//when the request finishes
		EErrorCode StatusCode;
		FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(req, res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("SendMutationRequest_HttpRequestComplete: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk && !JsonObject.HasField("errors")) {
			_TransactionHash = JsonObject.GetObjectField("data")->GetObjectField("submitTransaction")->GetStringField("transactionHash");
			if (!_TransactionHash.IsEmpty()) {
				this->WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->TimerHandle, this, &USendFutureverseARTM::GetARTMStatus, 3.0F, true, 1.0F);
			}
			else {
				//handle fail to parse errors
				OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
				UE_LOG(LogEmergence, Error, TEXT("Send Mutation Response parse failed"));
				return;
			}
		}
		else {
			//handle fail to parse errors
			OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			UE_LOG(LogEmergence, Error, TEXT("Send Mutation Request failed"));
			return;
		}
	});
	SendMutationRequest->ProcessRequest();
}

void USendFutureverseARTM::GetARTMStatus()
{
	GetTransactionStatusRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendFutureverseARTM>(
		this,
		nullptr,
		FEmergenceFutureverseAssetRegistryModule::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		60.0F,
		TArray<TPair<FString, FString>>(),
		FString(), false);
	GetTransactionStatusRequest->SetHeader("content-type", "application/json");
	FString ContentAsString = R"({"query":"query Transaction($transactionHash: TransactionHash!) {\n  transaction(transactionHash: $transactionHash) {\n    status\n    error {\n      code\n      message\n    }\n    events {\n      action\n      args\n      type\n    }\n  }\n}","variables":{"transactionHash":")" + _TransactionHash + R"("}})";
	GetTransactionStatusRequest->SetContentAsString(ContentAsString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ARTM Request Content: %s"), *ContentAsString);
	GetTransactionStatusRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		EErrorCode StatusCode;
		FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(req, res, bSucceeded, StatusCode);
		if (StatusCode == EErrorCode::EmergenceOk && !JsonObject.HasField("errors")) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Get ARTM Transaction: %s"), *res->GetContentAsString());
			FString TransactionStatus = JsonObject.GetObjectField("data")->GetObjectField("transaction")->GetStringField("status");
			UE_LOG(LogEmergenceHttp, Display, TEXT("Transaction status of \"%s\": %s"), *_TransactionHash, *TransactionStatus);
			if (TransactionStatus == "PENDING") {
				return;
			}
			else if (TransactionStatus == "SUCCESS") {
				OnSendARTMCompleted.Broadcast(_TransactionHash, EErrorCode::EmergenceOk);
				this->WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);			
			}
			else { //failed
				OnSendARTMCompleted.Broadcast(_TransactionHash, EErrorCode::EmergenceClientFailed);
				this->WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}	
			return;
		}
		else {
			this->WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			auto Errors = JsonObject.GetArrayField("errors");
			UE_LOG(LogEmergence, Error, TEXT("Get ARTM Transaction Status failed. Errors:"));
			for (auto Error : Errors) {
				UE_LOG(LogEmergence, Error, TEXT("%s"), *Error->AsObject()->GetStringField("message"));
			}
			OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			return;
		}
		});
	GetTransactionStatusRequest->ProcessRequest();
}