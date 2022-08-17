// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "WalletService/CreateKeyStoreFile.h"
#include "DatabaseService/SetActivePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreateKeyStoreFile *UCreateKeyStoreFile::CreateKeyStoreFile(
	const UObject *WorldContextObject,
	const FString &PrivateKey,
	const FString &Password,
	const FString &PublicKey,
	const FString &Path)
{
	UCreateKeyStoreFile *BlueprintNode = NewObject<UCreateKeyStoreFile>();
	BlueprintNode->PrivateKey = PrivateKey;
	BlueprintNode->Password = Password;
	BlueprintNode->PublicKey = PublicKey;
	BlueprintNode->Path = Path;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UCreateKeyStoreFile::Activate()
{
	Path = Path.Replace(TEXT(" "), TEXT("%20"));

	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreateKeyStoreFile>(
		this,
		&UCreateKeyStoreFile::CreateKeyStoreFile_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "createKeyStore" + "?privateKey=" + PrivateKey + "&password=" + Password + "&publicKey=" + PublicKey + "&path=" + Path,
		"POST",
		60.0F,
		Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("CreateKeyStoreFile request started, calling CreateKeyStoreFile_HttpRequestComplete on request completed"));
}

void UCreateKeyStoreFile::CreateKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk)
	{
		OnCreateKeyStoreFileCompleted.Broadcast(FString(), EErrorCode::EmergenceOk);
		return;
	}
	OnCreateKeyStoreFileCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("CreateKeyStoreFile", StatusCode);
}
