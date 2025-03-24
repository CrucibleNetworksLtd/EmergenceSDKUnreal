// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "Types/Transaction.h"
#include "Types/EmergenceChain.h"
#include "GetBlockNumber.generated.h"

UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UGetBlockNumber : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the current block number of given chain.
	 * @param Blockchain Blockchain to get the block number of.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetBlockNumber* GetBlockNumber(UObject* WorldContextObject, UEmergenceChain* Blockchain);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBlockNumberCompleted, int, BlockNumber, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetBlockNumberCompleted OnGetBlockNumberCompleted;
private:
	void GetBlockNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	UEmergenceChain* Blockchain;
};
