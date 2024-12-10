// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "CustodialLogin.generated.h"


USTRUCT()
struct FEmergenceCustodialLoginOutput //This only exists because I can't output a tmap from a dynamic multicast deligate
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FString> TokenData;

	FEmergenceCustodialLoginOutput() {};

	FEmergenceCustodialLoginOutput(TMap<FString, FString> _TokenData) {
		TokenData = _TokenData;
	};
};

UCLASS()
class EMERGENCE_API UCustodialLogin : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Begins a Futurepass custodial web login by opening the user's browser. When they complete this, EmergenceSingleton->OnLoginFinished will be called.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialLogin* CustodialLogin(const UObject* WorldContextObject);

	void Activate() override;

	void BeginDestroy() override;
private:

	UFUNCTION()
	FString CleanupBase64ForWeb(FString Input);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	bool HandleAuthRequestCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	UFUNCTION()
	FString GetSecureRandomBase64(int Length = 128);

	static bool _isServerStarted;

	static FHttpRouteHandle RouteHandle;

	static FString clientid;

	static const UObject* ContextObject;

	static FString code;

	static FString state;

	static FString GetClientID();
};
