// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "WalletService/CreateWalletAndKeyStoreFile.h"
#include "DatabaseService/SetActivePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreateWalletAndKeyStoreFile *UCreateWalletAndKeyStoreFile::CreateWalletAndKeyStoreFile(UObject *WorldContextObject, const FString &Path, const FString &Password)
{
	UCreateWalletAndKeyStoreFile *BlueprintNode = NewObject<UCreateWalletAndKeyStoreFile>();
	BlueprintNode->Path = Path;
	BlueprintNode->Password = Password;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCreateWalletAndKeyStoreFile::Activate()
{
#if UNREAL_MARKETPLACE_BUILD
	UE_LOG(LogEmergenceHttp, Error, TEXT("CreateWalletAndKeyStoreFile cannot be used on the Unreal Marketplace build."));
#else
	Path = Path.Replace(TEXT(" "), TEXT("%20"));

	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreateWalletAndKeyStoreFile>(
		this,
		&UCreateWalletAndKeyStoreFile::CreateWalletAndKeyStoreFile_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "createWallet?path=" + Path + "&password=" + Password,
		"POST",
		60.0F,
		Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("CreateWalletAndKeyStoreFile request started, calling CreateWalletAndKeyStoreFile_HttpRequestComplete on request completed"));
#endif
}

void UCreateWalletAndKeyStoreFile::CreateWalletAndKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk)
	{
		OnCreateWalletAndKeyStoreFileCompleted.Broadcast(FString(), EErrorCode::EmergenceOk);
	}
	else {
		OnCreateWalletAndKeyStoreFileCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("CreateWalletAndKeyStoreFile", StatusCode);
	}
}
