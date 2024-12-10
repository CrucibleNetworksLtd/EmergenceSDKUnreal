// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "ErrorCodeFunctionLibrary.h"
#include "CustodialSignMessage.generated.h"

//This class should only be called by Request To Sign
UCLASS(meta=(HasDedicatedAsyncNode))
class EMERGENCE_API UCustodialSignMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialSignMessage* CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message);

	void BeginDestroy() override;

	void SetReadyToDestroy() override;

	void Activate() override;

	TDelegate<void(FString, EErrorCode)> OnCustodialSignMessageComplete;
private:
	void LaunchSignMessageURL();

	static bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	static TUniquePtr<FHttpServerResponse> GetHttpPage();

	static TDelegate<void(FString, EErrorCode)> CallbackComplete;

	UPROPERTY()
	FString FVCustodialEOA;

	UPROPERTY()
	FString Message;

	static bool _isServerStarted;

	static FHttpRouteHandle RouteHandle;
};
