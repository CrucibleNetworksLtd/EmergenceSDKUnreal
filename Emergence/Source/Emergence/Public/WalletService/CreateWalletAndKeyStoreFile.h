// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "CreateWalletAndKeyStoreFile.generated.h"

UCLASS()
class EMERGENCE_API UCreateWalletAndKeyStoreFile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Creates a new wallet AND stores a keystore file.
	 * @param Path Where to store a keystore.
	 * @param Password The password that will encrypt our KeyStore File.
	 * @warning The directory that will contain the file does need to already exist, as only a file will be created.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Keystore Local Wallet")
	static UCreateWalletAndKeyStoreFile *CreateWalletAndKeyStoreFile(UObject *WorldContextObject, const FString &Path, const FString &Password);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateWalletAndKeyStoreFileCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCreateWalletAndKeyStoreFileCompleted OnCreateWalletAndKeyStoreFileCompleted;

private:
	FString Path;
	FString Password;
	void CreateWalletAndKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject *WorldContextObject;
};
