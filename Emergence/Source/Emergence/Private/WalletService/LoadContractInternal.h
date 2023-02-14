// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceDeployment.h"
#include "LoadContractInternal.generated.h"

UCLASS(NotBlueprintable)
class EMERGENCE_API ULoadContractInternal : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Loads a contract into the local server's memory.
	 * @param ContractAddress Address of the contract.
	 * @param ABI The Application Binary Interface of the given contract.
	 */
	UFUNCTION()
	static ULoadContractInternal* LoadContract(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract);

	void Activate();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadContractCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY()
	FOnLoadContractCompleted OnLoadContractCompleted;
private:
	void LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	UEmergenceDeployment* DeployedContract;
};
