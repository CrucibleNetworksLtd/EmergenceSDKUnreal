// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "LoadAccountFromKeyStoreFile.generated.h"

/**
 *
 */
UCLASS()
class EMERGENCE_API ULoadAccountFromKeyStoreFile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Loads a new account into the server and initializes a new web3 session.
	 * @param Name Assignable name to identify the account.
	 * @param Password The password of the keystore file.
	 * @param Path The path of the keystore file.
	 * @param NodeURL URL of the provider.
	 * @param ChainID The Chain ID, which must be compatible with the given NodeURL.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Keystore Local Wallet")
	static ULoadAccountFromKeyStoreFile* LoadAccountFromKeyStoreFile(const UObject* WorldContextObject, const FString& Name, const FString& Password, const FString& Path, const FString& NodeURL = "https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134", const FString& ChainID = "137");

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadAccountFromKeyStoreFileCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnLoadAccountFromKeyStoreFileCompleted OnLoadAccountFromKeyStoreFileCompleted;
private:
	void LoadAccountFromKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString Name;
	FString Password;
	FString Path;
	FString NodeURL;
	FString ChainID;
};
