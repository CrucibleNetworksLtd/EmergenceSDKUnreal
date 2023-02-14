// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/ValidateAccessToken.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UValidateAccessToken* UValidateAccessToken::ValidateAccessToken(UObject* WorldContextObject, const FString& AccessToken)
{
	UValidateAccessToken* BlueprintNode = NewObject<UValidateAccessToken>();
	BlueprintNode->AccessToken = AccessToken;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UValidateAccessToken::Activate()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UValidateAccessToken>(this, &UValidateAccessToken::ValidateAccessToken_HttpRequestComplete, UHttpHelperLibrary::APIBase + "validate-access-token" + "?accessToken=" + AccessToken);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ValidateAccessToken request started, calling ValidateAccessToken_HttpRequestComplete on request completed"));
}

void UValidateAccessToken::ValidateAccessToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);

	bool isValidToken = false;
	if (StatusCode == EErrorCode::EmergenceOk) {
		if (JsonObject.GetObjectField("message")->TryGetBoolField("valid", isValidToken)) {
			if (isValidToken)
			{
				UE_LOG(LogEmergenceHttp, Display, TEXT("Valid access token"));
			}
			else
			{
				UE_LOG(LogEmergenceHttp, Display, TEXT("Invalid access token"));
			}
			OnValidateAccessTokenCompleted.Broadcast(isValidToken, StatusCode);
		}
		else {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Unable to check access token validity"));
			OnValidateAccessTokenCompleted.Broadcast(isValidToken, StatusCode);
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ValidateAccessToken", StatusCode);
		}
	}
	else {
		OnValidateAccessTokenCompleted.Broadcast(isValidToken, StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ValidateAccessToken", StatusCode);
	}
	SetReadyToDestroy();
}
