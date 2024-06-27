// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "GetFuturepassInventoryByCollectionAndOwner.generated.h"


UCLASS()
class EMERGENCE_API UGetFuturepassInventoryByCollectionAndOwner : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the futurepass inventory of the given addresses, filted by the given collections.
	 * @param Addresses A list of addresses to get the futurepass inventory of - just the address, no blockchain information.
	 * @param Collections A list of collections to filter the inventory by - these should be in the format <chainID>:<chainType>:<Contract address>, e.g. "5:evm:0x1cac32d9893deca7769a2e64edc186163125d43b"
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse")
	static UGetFuturepassInventoryByCollectionAndOwner* GetFuturepassInventoryByCollectionAndOwner(UObject* WorldContextObject, TArray<FString> Addresses, TArray<FString> Collections);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFuturepassInventoryCompleted, FEmergenceInventory, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetFuturepassInventoryCompleted OnGetFuturepassInventoryCompleted;

private:
	TArray<FString> Addresses, Collections;
};
