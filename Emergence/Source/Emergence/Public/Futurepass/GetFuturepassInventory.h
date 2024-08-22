// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "GetFuturepassInventory.generated.h"


UCLASS()
class EMERGENCE_API UGetFuturepassInventory : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the futurepass inventory of the given addresses.
	 * @param Addresses A list of addresses to get the futurepass inventory of - just the address, no blockchain information.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse")
	static UGetFuturepassInventory* GetFuturepassInventory(UObject* WorldContextObject, TArray<FString> Addresses);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFuturepassInventoryCompleted, FEmergenceInventory, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetFuturepassInventoryCompleted OnGetFuturepassInventoryCompleted;

private:
	TArray<FString> Addresses;
};
