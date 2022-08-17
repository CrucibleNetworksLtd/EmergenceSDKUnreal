// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "WriteMethod.generated.h"

UCLASS()
class EMERGENCE_API UWriteMethod : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "write" method on the given contract.
	 * @param ContractAddress Address of the contract.
	 * @param MethodName The method to call.
	 * @param Value The amount to transfer from sender to recipient (in Wei, or equivelent).
	 * @param Content The parameters to call the method with.
	 * @param LocalAccountName The local account to call the method with. Leave blank to call with the WalletConnect'd wallet.
	 * @param GasPrice The gas price to use if we're calling with a local account.
	 * @warning Make sure the local server already knows about the contract by calling LoadContract first!
	 */
	UFUNCTION(BlueprintCallable, meta = (Value = "0", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UWriteMethod* WriteMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, FString Value, TArray<FString> Content, FString LocalAccountName, FString GasPrice);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWriteMethodCompleted, FString, TransactionHash, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnWriteMethodCompleted OnWriteMethodCompleted;
private:
	void WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress;
	FString MethodName;
	TArray<FString> Content;
	FString LocalAccountName;
	FString GasPrice;
	FString Value;
};
