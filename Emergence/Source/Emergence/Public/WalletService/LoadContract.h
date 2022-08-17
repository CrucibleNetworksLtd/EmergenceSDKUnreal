// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "LoadContract.generated.h"

UCLASS()
class EMERGENCE_API ULoadContract : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Loads a contract into the local server's memory.
	 * @param ContractAddress Address of the contract.
	 * @param ABI The Application Binary Interface of the given contract.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static ULoadContract* LoadContract(const UObject* WorldContextObject, FString ContractAddress, FString ABI);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadContractCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnLoadContractCompleted OnLoadContractCompleted;
private:
	void LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress;
	FString ABI;
};
