// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/RequestToSign.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

URequestToSign* URequestToSign::RequestToSign(UObject* WorldContextObject, const FString& MessageToSign)
{
	URequestToSign* BlueprintNode = NewObject<URequestToSign>();
	BlueprintNode->MessageToSign = MessageToSign;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void URequestToSign::Activate()
{
	auto Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	
	//"Web login" flow stuff
	if (Singleton->UsingWebLoginFlow) {
		MessageToSign.ReplaceInline(TEXT("\\\""), TEXT("\"")); //unescape double layered json
		CustodialSignMessage = UCustodialSignMessage::CustodialSignMessage(WorldContextObject, Singleton->GetCachedAddress(true), MessageToSign);
		CustodialSignMessage->OnCustodialSignMessageComplete.BindLambda([&](const FString SignedMessage, EErrorCode StatusCode){
			if (StatusCode == EErrorCode::EmergenceOk) {
				UE_LOG(LogEmergence, Display, TEXT("OnInternalCustodialSignMessageComplete sucessfully"));
				OnRequestToSignCompleted.Broadcast(SignedMessage, StatusCode);
			}
			else {
				OnRequestToSignCompleted.Broadcast("", StatusCode);
				UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("RequestToSign", StatusCode);
			}
			SetReadyToDestroy();
		});
		CustodialSignMessage->Activate();
		UE_LOG(LogEmergenceHttp, Display, TEXT("RequestToSign request started via web login flow, calling OnInternalCustodialSignMessageComplete on request completed"));
	}
	//wallet connect flow
	else {
		FString Content = "{\"message\": \"" + MessageToSign + "\"}";
		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
		Headers.Add(TPair<FString, FString>("accept", "text/plain"));

		if (!Singleton->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
			Headers.Add(TPair<FString, FString>("deviceId", Singleton->DeviceID));
		}

		Request = UHttpHelperLibrary::ExecuteHttpRequest<URequestToSign>(this, &URequestToSign::RequestToSign_HttpRequestComplete, UHttpHelperLibrary::APIBase + "request-to-sign", "POST", 60.0F, Headers, Content);
		UE_LOG(LogEmergenceHttp, Display, TEXT("RequestToSign request started, calling RequestToSign_HttpRequestComplete on request completed"));
	}
}

void URequestToSign::BeginDestroy()
{
	UEmergenceAsyncSingleRequestBase::BeginDestroy();
}

void URequestToSign::RequestToSign_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	bool isValidToken = false;
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Message = "";
		Message = JsonObject.GetObjectField("message")->GetStringField("signedMessage");
		if (Message != "") {
			OnRequestToSignCompleted.Broadcast(Message, StatusCode);
		}
		else {
			OnRequestToSignCompleted.Broadcast("", EErrorCode::EmergenceInternalError);
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("RequestToSign", StatusCode);
		}
	}
	else {
		OnRequestToSignCompleted.Broadcast("", StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("RequestToSign", StatusCode);
	}
	SetReadyToDestroy();
}

void URequestToSign::OnInternalCustodialSignMessageComplete(const FString SignedMessage, EErrorCode StatusCode)
{

}
