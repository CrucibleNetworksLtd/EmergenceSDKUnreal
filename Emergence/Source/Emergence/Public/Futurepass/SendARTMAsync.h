// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceCancelableAsyncBase.h"
#include "Futurepass/ARTMBuilderLibrary.h"
#include "ErrorCodeFunctionLibrary.h"
#include "SendARTMAsync.generated.h"


UCLASS()
class EMERGENCE_API USendFutureverseARTM : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	/**
	 * Sends an ARTM, including creating the ARTM message, sending it to the wallet to be signed, sending it to the ARTM endpoint and waiting for the transaction to succeed or fail.
	 * @param Message The message to include as part of the ARTM, tells the user what is happening.
	 * @param ARTMOperations The operations to perform as part of the ARTM, such as creating or deleting links.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse")
	static USendFutureverseARTM* SendFutureverseARTM(UObject* WorldContextObject, FString Message, TArray<FFutureverseARTMOperation> ARTMOperations);

	virtual void Activate() override;

	virtual void Cancel();

	virtual bool IsActive() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSendARTMCompleted, FString, TransactionHash, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnSendARTMCompleted OnSendARTMCompleted;

private:
	FString ConstructedMessage;
	FString _MessageToUser;
	TArray<FFutureverseARTMOperation> _ARTMOperations;
	FString _TransactionHash;

	FHttpRequestPtr GetNonceRequest;
	FHttpRequestPtr RequestToSignRequest;
	FHttpRequestPtr SendMutationRequest;
	FHttpRequestPtr GetTransactionStatusRequest;

	FTimerHandle TimerHandle;

	UFUNCTION()
	void GetARTMStatus();

	UFUNCTION()
	void OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode);
};
