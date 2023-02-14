// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "AvatarService/AvatarById.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UAvatarById* UAvatarById::AvatarById(UObject* WorldContextObject, const FString& AvatarIdString)
{
	UAvatarById* BlueprintNode = NewObject<UAvatarById>();
	BlueprintNode->AvatarIdString = FString(AvatarIdString);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UAvatarById::Activate()
{
	FString requestURL = UHttpHelperLibrary::GetAvatarServiceAPIURL() + "id?id=" + AvatarIdString;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Host", UHttpHelperLibrary::GetAvatarServiceHostURL()});

	UHttpHelperLibrary::ExecuteHttpRequest<UAvatarById>(
		this,
		&UAvatarById::AvatarById_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("AvatarById request started, calling AvatarById_HttpRequestComplete on request completed"));
}

void UAvatarById::AvatarById_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);

	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergenceAvatarResult Result;
		FEmergenceAvatarMetadata Metadata;
		if (FJsonObjectConverter::JsonObjectToUStruct<FEmergenceAvatarResult>(JsonObject.GetObjectField("message").ToSharedRef(), &Result, 0, 0) &&
			FJsonObjectConverter::JsonObjectToUStruct<FEmergenceAvatarMetadata>(JsonObject.GetObjectField("message").ToSharedRef(), &Metadata, 0, 0)
			) {
			FEmergenceAvatarData AvatarData;
			AvatarData.AvatarNFT = Result;
			AvatarData.Avatar = Metadata;
			OnAvatarByIdCompleted.Broadcast(AvatarData, StatusCode);
		}
		else {
			OnAvatarByIdCompleted.Broadcast(FEmergenceAvatarData(), EErrorCode::EmergenceClientJsonParseFailed);
		}
	}
	else {
		OnAvatarByIdCompleted.Broadcast(FEmergenceAvatarData(), StatusCode);
	}
	SetReadyToDestroy();
}