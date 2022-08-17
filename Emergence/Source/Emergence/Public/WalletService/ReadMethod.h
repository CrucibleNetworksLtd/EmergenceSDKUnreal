// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "ReadMethod.generated.h"

UCLASS()
class EMERGENCE_API UReadMethod : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "read" method on the given contract.
	 * @param ContractAddress Address of the contract.
	 * @param MethodName The method to call.
	 * @param Content The parameters to call the method with.
	 * @param CustomNodeURL A custom node URL to call the method on. Leave blank for the relevent one from the project settings.
	 * @warning Make sure the local server already knows about the contract by calling LoadContract first!
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UReadMethod* ReadMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, TArray<FString> Content, FString CustomNodeURL);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReadMethodCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnReadMethodCompleted OnReadMethodCompleted;
private:
	void ReadMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress, CustomNodeURL, MethodName;
	TArray<FString> Content;
};
