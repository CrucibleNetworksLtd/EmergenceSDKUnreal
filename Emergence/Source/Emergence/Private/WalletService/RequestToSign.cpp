// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/RequestToSign.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

URequestToSign* URequestToSign::RequestToSign(const UObject* WorldContextObject, const FString& MessageToSign)
{
	URequestToSign* BlueprintNode = NewObject<URequestToSign>();
	BlueprintNode->MessageToSign = MessageToSign;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void URequestToSign::Activate()
{
	FString Content = "{\"message\": \"" + MessageToSign + "\"}";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Content);
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
	Headers.Add(TPair<FString, FString>("accept", "text/plain"));
	UHttpHelperLibrary::ExecuteHttpRequest<URequestToSign>(this, &URequestToSign::RequestToSign_HttpRequestComplete, UHttpHelperLibrary::APIBase + "request-to-sign", "POST", 60.0F, Headers, Content);
	UE_LOG(LogEmergenceHttp, Display, TEXT("RequestToSign request started, calling RequestToSign_HttpRequestComplete on request completed"));
}

void URequestToSign::RequestToSign_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
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
		return;
	}
	OnRequestToSignCompleted.Broadcast("", StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("RequestToSign", StatusCode);
}