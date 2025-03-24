// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "Types/Transaction.h"
#include "Types/EmergenceChain.h"
#include "GetTransactionStatus.generated.h"

UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UGetTransactionStatus : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the status of the transaction.
	 * @param TransactionHash Hash of the transaction.
	 * @param Blockchain Blockchain to get the transaction status from.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetTransactionStatus* GetTransactionStatus(UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* Blockchain);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionStatusCompleted, FEmergenceTransaction, Transaction, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTransactionStatusCompleted OnGetTransactionStatusCompleted;
private:
	void GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	FString TransactionHash;
	UEmergenceChain* Blockchain;
};
