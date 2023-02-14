// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/ValidateSignedMessage.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UValidateSignedMessage* UValidateSignedMessage::ValidateSignedMessage(UObject* WorldContextObject, const FString& Message, const FString& SignedMessage, const FString& Address)
{
	UValidateSignedMessage* BlueprintNode = NewObject<UValidateSignedMessage>();
	BlueprintNode->Message = Message;
	BlueprintNode->SignedMessage = SignedMessage;
	BlueprintNode->Address = Address;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UValidateSignedMessage::Activate()
{
	FString Content = "{\"message\": \"" + Message + "\", \"signedMessage\": \"" + SignedMessage + "\", \"address\": \"" + Address + "\"}";
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
	Headers.Add(TPair<FString, FString>("accept", "text/plain"));
	UHttpHelperLibrary::ExecuteHttpRequest<UValidateSignedMessage>(this, &UValidateSignedMessage::ValidateSignedMessage_HttpRequestComplete, UHttpHelperLibrary::APIBase + "validate-signed-message" + "?signedMessage=" + SignedMessage + "&address=" + Address, "POST", 60.0F, Headers, Content);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ValidateSignedMessage request started, calling ValidateSignedMessage_HttpRequestComplete on request completed"));
}

void UValidateSignedMessage::ValidateSignedMessage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);

	bool isValidToken = false;
	if (StatusCode == EErrorCode::EmergenceOk) {
		if (JsonObject.GetObjectField("message")->TryGetBoolField("valid", isValidToken)) {
			if (isValidToken)
			{
				UE_LOG(LogEmergenceHttp, Display, TEXT("Valid signed message"));
			}
			else
			{
				UE_LOG(LogEmergenceHttp, Display, TEXT("Invalid signed message"));
			}
			OnValidateSignedMessageCompleted.Broadcast(isValidToken, StatusCode);
		}
		else {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Unable to check access token validity"));
			OnValidateSignedMessageCompleted.Broadcast(isValidToken, StatusCode);
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ValidateSignedMessage", StatusCode);
		}
	}
	else {
		OnValidateSignedMessageCompleted.Broadcast(isValidToken, StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ValidateSignedMessage", StatusCode);
	}
	SetReadyToDestroy();
}
