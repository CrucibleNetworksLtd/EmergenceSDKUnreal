// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "AvatarService/AvatarByOwner.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceInventory.h"

UAvatarByOwner* UAvatarByOwner::AvatarByOwner(UObject* WorldContextObject, const FString& Address)
{
	UAvatarByOwner* BlueprintNode = NewObject<UAvatarByOwner>();
	BlueprintNode->Address = FString(Address);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UAvatarByOwner::Activate()
{
	FString requestURL = FEmergenceInventoryModule::GetAvatarServiceAPIURL() + "byOwner?address=" + Address;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Host", FEmergenceInventoryModule::GetAvatarServiceHostURL()});

	Request = UHttpHelperLibrary::ExecuteHttpRequest<UAvatarByOwner>(
		this,
		&UAvatarByOwner::AvatarByOwner_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("AvatarByOwner request started, calling AvatarByOwner_HttpRequestComplete on request completed"));
}

void UAvatarByOwner::AvatarByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);

	if (StatusCode == EErrorCode::EmergenceOk) {
		FJsonObjectConverter::JsonArrayToUStruct<FEmergenceAvatarResult>(JsonObject.GetArrayField("message"), &this->Results, 0, 0);
		
		if (JsonObject.GetArrayField("message").Num() == 0) {
			OnAvatarByOwnerCompleted.Broadcast(TArray<FEmergenceAvatarResult>(), StatusCode);
			return;
		}

		for (int i = 0; i < JsonObject.GetArrayField("message").Num(); i++) {
			FString TokenURI = JsonObject.GetArrayField("message")[i].Get()->AsObject().Get()->GetStringField("tokenURI");
			if (!TokenURI.IsEmpty()) {
				auto NewRequest = UHttpHelperLibrary::ExecuteHttpRequest<UAvatarByOwner>(this, &UAvatarByOwner::GetMetadata_HttpRequestComplete, TokenURI);
				Requests.Add(TPair<FHttpRequestRef, FEmergenceAvatarResult>(NewRequest, Results[i]));
			}
			else {
				UE_LOG(LogEmergenceHttp, Warning, TEXT("One of the Avatar's TokenURI was blank in the AvatarByOwner, ignoring it."));
			}
		}
		return;
	}

	OnAvatarByOwnerCompleted.Broadcast(TArray<FEmergenceAvatarResult>(), StatusCode);
	SetReadyToDestroy();
}

void UAvatarByOwner::GetMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	auto ThisRequest = Requests.FindByPredicate([HttpRequest](TPair<FHttpRequestRef, FEmergenceAvatarResult> RequestToFind) {
		return RequestToFind.Key.Get().GetURL() == HttpRequest.Get()->GetURL();
	});

	FJsonObjectConverter::JsonArrayStringToUStruct<FEmergenceAvatarMetadata>(*HttpResponse->GetContentAsString(), &ThisRequest->Value.Avatars, 0, 0);
	this->Results.Add(ThisRequest->Value);


	for (int i = 0; i < Requests.Num(); i++) {
		if (Requests[i].Key.Get().GetStatus() == EHttpRequestStatus::Processing) {
			return;
		}
	}
	OnAvatarByOwnerCompleted.Broadcast(this->Results, EErrorCode::EmergenceOk);
	SetReadyToDestroy();
}
