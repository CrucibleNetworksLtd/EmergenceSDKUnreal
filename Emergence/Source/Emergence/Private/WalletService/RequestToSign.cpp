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
	FString Content = "{\"message\": \"" + MessageToSign + "\"}";
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
	Headers.Add(TPair<FString, FString>("accept", "text/plain"));

	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId",UEmergenceSingleton::DeviceID));
	}

	UHttpHelperLibrary::ExecuteHttpRequest<URequestToSign>(this, &URequestToSign::RequestToSign_HttpRequestComplete, UHttpHelperLibrary::APIBase + "request-to-sign", "POST", 60.0F, Headers, Content);
	UE_LOG(LogEmergenceHttp, Display, TEXT("RequestToSign request started, calling RequestToSign_HttpRequestComplete on request completed"));
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
