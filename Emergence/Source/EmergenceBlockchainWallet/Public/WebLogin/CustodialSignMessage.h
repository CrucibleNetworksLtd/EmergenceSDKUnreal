// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "Types/EmergenceErrorCode.h"
#include "CustodialSignMessage.generated.h"

//This class should only be called by Request To Sign
UCLASS(meta=(HasDedicatedAsyncNode))
class EMERGENCEBLOCKCHAINWALLET_API UCustodialSignMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION()
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
