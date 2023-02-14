// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/InventoryByOwner.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UInventoryByOwner* UInventoryByOwner::InventoryByOwner(UObject* WorldContextObject, const FString& Address, const FString& Network)
{
	UInventoryByOwner* BlueprintNode = NewObject<UInventoryByOwner>();
	BlueprintNode->Address = FString(Address);
	BlueprintNode->Network = FString(Network);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UInventoryByOwner::Activate()
{
	FString requestURL = UHttpHelperLibrary::GetInventoryServiceAPIURL() + "byOwner?address=" + Address + "&network=" + Network;
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Host", UHttpHelperLibrary::GetInventoryServiceHostURL()});

	UHttpHelperLibrary::ExecuteHttpRequest<UInventoryByOwner>(
		this,
		&UInventoryByOwner::InventoryByOwner_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("InventoryByOwner request started, calling InventoryByOwner_HttpRequestComplete on request completed"));
}

void UInventoryByOwner::InventoryByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);

	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergenceInventory Inventory;
		FJsonObjectConverter::JsonObjectToUStruct<FEmergenceInventory>(
			JsonObject.GetObjectField("message").ToSharedRef(),
			&Inventory,
			0, 0);
		OnInventoryByOwnerCompleted.Broadcast(Inventory, EErrorCode::EmergenceOk);
	}
	else {
		OnInventoryByOwnerCompleted.Broadcast(FEmergenceInventory(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("InventoryByOwner", StatusCode);
	}
	SetReadyToDestroy();
}
