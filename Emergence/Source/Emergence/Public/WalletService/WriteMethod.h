// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceCancelableAsyncBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceDeployment.h"
#include "WalletService/GetTransactionStatus.h"
#include "Engine/EngineTypes.h"
#include "Transaction.h"
#include "WriteMethod.generated.h"

UCLASS()
class EMERGENCE_API UWriteMethod : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "write" method on the given contract. OnTransactionSent is executed when the transaction is sent by the connected wallet sucessfully; you must wait for OnTransactionConfirmed to return to use ConfirmedTransaction or FinalStatusCode (all error handling should be done after OnTransactionConfirmed).
	 * @param DeployedContract The deployed contract.
	 * @param MethodName The method to call.
	 * @param Value The amount to transfer from sender to recipient (in Wei, or equivelent).
	 * @param Content The parameters to call the method with.
	 * @param PrivateKey (Optional) The private key of the account to call the method with. Leave blank to call with the WalletConnect'd wallet / Futureverse custodial wallet.
	 * @param GasPrice (Optional) The gas price to use if we're calling with a local account.
	 * @param NumberOfConfirmations The number of confirmations to wait for before deciding this transaction "confirmed".
	 * @param TimeBetweenChecks The amount of time between checks in seconds.
	 */
	UFUNCTION(BlueprintCallable, meta = (Value = "0", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Content"), Category = "Emergence|Blockchain Interactions")
	static UWriteMethod* WriteMethod(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, FString Value, TArray<FString> Content, FString PrivateKey, FString GasPrice, int NumberOfConfirmations = 5, float TimeBetweenChecks = 5.0F);

	virtual void Activate() override;

	UFUNCTION()
	void LoadContractCompleted(FString Response, EErrorCode StatusCode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransactionConfirmed, FEmergenceTransaction, ConfirmedTransaction, EErrorCode, FinalStatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnTransactionConfirmed OnTransactionConfirmed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransactionSent);

	UPROPERTY(BlueprintAssignable)
	FOnTransactionSent OnTransactionSent;

	UFUNCTION()
	void CallWriteMethod();

	virtual void Cancel();

	virtual bool IsActive() const;

	FHttpRequestPtr LoadContractRequest;
	FHttpRequestPtr SwitchChainRequest;
	FHttpRequestPtr WriteMethodRequest;

	void SendTransactionViaKeystoreComplete(FString Response);
private:
	void WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	UEmergenceDeployment* DeployedContract;
	FEmergenceContractMethod MethodName;
	TArray<FString> Content;
	FString LocalAccountName;
	FString GasPrice;
	FString Value;
	FString TransactionHash;
	int NumberOfConfirmations;
	float TimeBetweenChecks;
	FTimerHandle TimerHandle;

	UFUNCTION()
	void GetTransactionStatus();

	UFUNCTION()
	void CustodialWriteTransactionCompleted(FString TransactionHash, EErrorCode StatusCode);

	UFUNCTION()
	void TransactionStatusReturns(FEmergenceTransaction Transaction, EErrorCode StatusCode);

	UFUNCTION()
	void WaitThenGetStatus();
};
