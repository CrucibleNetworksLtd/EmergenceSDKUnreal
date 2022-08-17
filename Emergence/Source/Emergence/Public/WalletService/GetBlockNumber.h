// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "Transaction.h"
#include "GetBlockNumber.generated.h"

UCLASS()
class EMERGENCE_API UGetBlockNumber : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the current block number of given chain.
	 * @param nodeURL nodeURL of the chain. Leave blank for nodeURL from Emergence project settings.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetBlockNumber* GetBlockNumber(const UObject* WorldContextObject, FString NodeURL);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBlockNumberCompleted, int, BlockNumber, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetBlockNumberCompleted OnGetBlockNumberCompleted;
private:
	void GetBlockNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString NodeURL;
};
