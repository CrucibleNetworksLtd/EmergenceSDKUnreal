// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EmergenceInventoryServiceStructs.h"
#include "InventoryByOwner.generated.h"


UCLASS()
class EMERGENCE_API UInventoryByOwner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the inventory (owned tokens) from a given address.
	 * @param Address Address to get the inventory from.
	 * @param Network Comma seperated list of networks to get the inventory from. To get from all available, enter "ETHEREUM,POLYGON,FLOW,TEZOS,SOLANA,IMMUTABLEX".
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Inventory Service")
	static UInventoryByOwner* InventoryByOwner(UObject* WorldContextObject, const FString& Address, const FString& Network = "ETHEREUM,POLYGON,FLOW,TEZOS,SOLANA,IMMUTABLEX");

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryByOwnerCompleted, FEmergenceInventory, Inventory, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnInventoryByOwnerCompleted OnInventoryByOwnerCompleted;
private:
	void InventoryByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Address, Network;
	UObject* WorldContextObject;
};
