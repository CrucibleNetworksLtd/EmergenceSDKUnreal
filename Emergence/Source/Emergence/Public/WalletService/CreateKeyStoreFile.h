// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "CreateKeyStoreFile.generated.h"

UCLASS()
class EMERGENCE_API UCreateKeyStoreFile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Creates a new KeyStore file using an existing private key.
	 * @param Path Where to store a keystore.
	 * @param Password The password that will encrypt our KeyStore File.
	 * @param PublicKey The public key to use to create the KeyStore.
	 * @param PrivateKey The private key to use to create the KeyStore.
	 * @warning The directory that will contain the file does need to already exist, as only a file will be created.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Keystore Local Wallet")
	static UCreateKeyStoreFile *CreateKeyStoreFile(UObject *WorldContextObject, const FString &PrivateKey, const FString &Password, const FString &PublicKey, const FString &Path);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateKeyStoreFileCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCreateKeyStoreFileCompleted OnCreateKeyStoreFileCompleted;

private:
	FString PrivateKey;
	FString Password;
	FString PublicKey;
	FString Path;
	void CreateKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject *WorldContextObject;
};
