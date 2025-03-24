// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceCancelableAsyncBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "Types/EmergenceDeployment.h"
#include "JsonObjectWrapper.h"
#include "ReadMethod.generated.h"

UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UReadMethod : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "read" method on the given contract.
	 * @param DeployedContract The deployed contract.
	 * @param MethodName The method to call.
	 * @param Parameters The parameters to call the method with.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Parameters"), Category = "Emergence|Blockchain Interactions")
	static UReadMethod* ReadMethod(UObject * WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, TArray<FString> Parameters);

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
	TArray<FString> Parameters;
};
