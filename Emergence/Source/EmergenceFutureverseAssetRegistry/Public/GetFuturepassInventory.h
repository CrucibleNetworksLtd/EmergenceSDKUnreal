// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "Types/EmergenceInventory.h"
#include "GetFuturepassInventory.generated.h"


UCLASS()
class EMERGENCEFUTUREVERSEASSETREGISTRY_API UGetFuturepassInventory : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the futurepass inventory of the given addresses, optionally filtered by the given collections. If no collections are provided, it returns all collections.
	 * @param Addresses A list of addresses to get the futurepass inventory of - just the address, no blockchain information.
	 * @param Collections An optional list of collections to filter the inventory by - these should be in the format <chainID>:<chainType>:<Contract address>, e.g. "5:evm:0x1cac32d9893deca7769a2e64edc186163125d43b". If no collections are provided, it returns all collections.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Collections"), Category = "Emergence|Futureverse|Asset Registry")
	static UGetFuturepassInventory* GetFuturepassInventory(UObject* WorldContextObject, TArray<FString> Addresses, TArray<FString> Collections);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetFuturepassInventoryCompleted, FEmergenceInventory, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetFuturepassInventoryCompleted OnGetFuturepassInventoryCompleted;

private:
	TArray<FString> Addresses, Collections;
};
