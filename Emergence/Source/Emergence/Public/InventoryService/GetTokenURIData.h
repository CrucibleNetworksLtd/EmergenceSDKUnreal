// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "GetTokenURIData.generated.h"


UCLASS()
class EMERGENCE_API UGetTokenURIData : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Interface Helpers")
	static UGetTokenURIData* GetTokenURIData(const UObject* WorldContextObject, const FString& TokenURI);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenURIDataCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTokenURIDataCompleted OnGetTokenURIDataCompleted;
private:
	void GetTokenURIData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString TokenURI;
	const UObject* WorldContextObject;
};