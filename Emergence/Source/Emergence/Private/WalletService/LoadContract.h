// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ErrorCodeFunctionLibrary.h"
#include "LoadContract.generated.h"

UCLASS()
class EMERGENCE_API ULoadContract : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * THIS NODE IS DEPRECATED. This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP.
	 * @param ContractAddress Address of the contract.
	 * @param ABI The Application Binary Interface of the given contract.
	 */
	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage="This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP.", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static ULoadContract* LoadContract(UObject* WorldContextObject, FString ContractAddress, FString ABI);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadContractCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, meta = (DeprecatedProperty, DeprecationMessage="This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP."))
	FOnLoadContractCompleted OnLoadContractCompleted;
};
