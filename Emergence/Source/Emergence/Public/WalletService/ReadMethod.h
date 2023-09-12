// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceCancelableAsyncBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceDeployment.h"
#include "JsonObjectWrapper.h"
#include "ReadMethod.generated.h"

UCLASS()
class EMERGENCE_API UReadMethod : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "read" method on the given contract.
	 * @param DeployedContract The deployed contract.
	 * @param MethodName The method to call.
	 * @param Content The parameters to call the method with.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Content"), Category = "Emergence|Blockchain Interactions")
	static UReadMethod* ReadMethod(UObject * WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, TArray<FString> Content);

	virtual void Activate() override;

	UFUNCTION()
	void LoadContractCompleted(FString Response, EErrorCode StatusCode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReadMethodCompleted, FJsonObjectWrapper, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnReadMethodCompleted OnReadMethodCompleted;

	virtual void Cancel();

	virtual bool IsActive() const;

	FHttpRequestPtr LoadContractRequest;
	FHttpRequestPtr ReadMethodRequest;
private:
	void ReadMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	FEmergenceContractMethod MethodName;
	UEmergenceDeployment* DeployedContract;
	TArray<FString> Content;
};
