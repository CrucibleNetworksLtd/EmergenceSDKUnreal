// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceSingleton.h"
#include "Engine/Engine.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "GameDelegates.h"
#include "Engine/Texture2D.h"

//for HTTP services
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"

#include "HttpService/HttpHelperLibrary.h"
#include "Containers/UnrealString.h"

#include "EmergenceChainObject.h"

UEmergenceSingleton::UEmergenceSingleton() {
}

TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> UEmergenceSingleton::GlobalManagers{};

UEmergenceSingleton* UEmergenceSingleton::GetEmergenceManager(const UObject* ContextObject)
{
	if (!GEngine || !ContextObject) {
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (GameInstance)
	{
		TWeakObjectPtr<UEmergenceSingleton>& Manager = GlobalManagers.FindOrAdd(GameInstance);
		if (!Manager.IsValid())
		{
			Manager = NewObject<UEmergenceSingleton>(GameInstance);
			Manager->SetGameInstance(GameInstance);
			Manager->Init();
		}
		UE_LOG(LogEmergenceHttp, Verbose, TEXT("Got Emergence Singleton: %s"), *Manager->GetFName().ToString());
		return Manager.Get();
	}
	UE_LOG(LogEmergenceHttp, Error, TEXT("Text %s"), "No manager avalible, whats going on?");
	return nullptr;
}

UEmergenceSingleton* UEmergenceSingleton::ForceInitialize(const UObject* ContextObject)
{
	return GetEmergenceManager(ContextObject);
}

void UEmergenceSingleton::Init()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().AddUObject(this, &UEmergenceSingleton::Shutdown);
	AddToRoot();
}

void UEmergenceSingleton::Shutdown()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().RemoveAll(this);

	RemoveFromRoot();
	MarkPendingKill();
}

void UEmergenceSingleton::SetCachedCurrentPersona(FEmergencePersona NewCachedCurrentPersona)
{
	this->CachedCurrentPersona = NewCachedCurrentPersona;
	OnCachedPersonaUpdated.Broadcast(this->CachedCurrentPersona);
}

void UEmergenceSingleton::SetOwnedAvatarNFTCache(TArray<FEmergenceAvatarResult> Results)
{
	this->OwnedAvatarNFTCache = Results;
	this->OwnedAvatarNFTCached = true;
	this->OnOwnedAvatarNFTCacheUpdated.Broadcast();
}

void UEmergenceSingleton::FlushOwnedAvatarNFTCache()
{
	this->OwnedAvatarNFTCache.Empty();
	this->OwnedAvatarNFTCached = false;
}

bool UEmergenceSingleton::HandleDatabaseServerAuthFail(EErrorCode ErrorCode)
{
	if (ErrorCode == EErrorCode::Denied) {
		OnDatabaseAuthFailed.Broadcast();
		return true;
	}
	else{
		return false;
	}
}

//HTTP Services
void UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			UE_LOG(LogEmergenceHttp, Display, TEXT("GetWalletConnectURI request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnGetWalletConnectURIRequestCompleted.Broadcast(*ResponseStr, EErrorCode::EmergenceOk);
			return;
		}
	}
	OnGetWalletConnectURIRequestCompleted.Broadcast(FString(), UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded));
	OnAnyRequestError.Broadcast("GetWalletConnectURI", UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded));
}

void UEmergenceSingleton::CancelSignInRequest()
{
	if (GetAccessTokenRequest && GetAccessTokenRequest->GetStatus() == EHttpRequestStatus::Processing) {
		GetAccessTokenRequest->CancelRequest();
	}

	if (GetHandshakeRequest && GetHandshakeRequest->GetStatus() == EHttpRequestStatus::Processing) {
		GetHandshakeRequest->CancelRequest();
	}
}

FString UEmergenceSingleton::GetCurrentAccessToken()
{
	if (this->CurrentAccessToken.Len() > 0) {
		return this->CurrentAccessToken;
	}
	else {
		GetAccessToken();
		return FString("-1");
	}
}

UEmergenceUI* UEmergenceSingleton::OpenEmergenceUI(APlayerController* OwnerPlayerController, TSubclassOf<UEmergenceUI> EmergenceUIClass)
{
	if (EmergenceUIClass) {
		CurrentEmergenceUI = CreateWidget<UEmergenceUI>(OwnerPlayerController, EmergenceUIClass);
		CurrentEmergenceUI->AddToViewport(9999);

		//Get the current state of showing the mouse so we can set it back to this later
		this->PreviousMouseShowState = OwnerPlayerController->bShowMouseCursor;
		//Get the current state of input mode so we can set it back to this later
		UGameViewportClient* GameViewportClient = OwnerPlayerController->GetWorld()->GetGameViewport();
		bool IgnoringInput = GameViewportClient->IgnoreInput();
		EMouseCaptureMode CaptureMouse = GameViewportClient->GetMouseCaptureMode();

		if (IgnoringInput == false && CaptureMouse == EMouseCaptureMode::CaptureDuringMouseDown) //Game And UI
		{
			this->PreviousGameInputMode = 0;
		}
		else if (IgnoringInput == true && CaptureMouse == EMouseCaptureMode::NoCapture) //UI Only
		{
			this->PreviousGameInputMode = 1;
		}
		else //Game Only
		{
			this->PreviousGameInputMode = 2;
		}

		OwnerPlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode = FInputModeUIOnly();
		InputMode.SetWidgetToFocus(CurrentEmergenceUI->GetCachedWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		OwnerPlayerController->SetInputMode(InputMode);

		CurrentEmergenceUI->Closed.AddDynamic(this, &UEmergenceSingleton::OnOverlayClosed);

		if (CurrentEmergenceUI) {
			return CurrentEmergenceUI;
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
	
}

UEmergenceUI* UEmergenceSingleton::GetEmergenceUI()
{
	if (CurrentEmergenceUI->IsValidLowLevel()) {
		return CurrentEmergenceUI;
	}
	else {
		return nullptr;
	}
}

bool UEmergenceSingleton::HasAccessToken()
{
	return this->CurrentAccessToken != FString("");
}

bool UEmergenceSingleton::HasCachedAddress()
{
	return this->CurrentAddress != FString("");
}

FString UEmergenceSingleton::GetCachedAddress()
{
	if (this->CurrentAddress.Len() > 0) {
		return this->CurrentAddress;
	}
	else {
		return FString("-1");
	}
}

void UEmergenceSingleton::GetWalletConnectURI()
{
	this->DeviceID = ""; //clear the device ID, we'll be getting a new one so we don't want to be able to accidently send an old header

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete, UHttpHelperLibrary::APIBase + "getwalletconnecturi");
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetWalletConnectURI request started, calling GetWalletConnectURI_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::CallRequestError(FString ConnectionName, EErrorCode StatusCode)
{
	this->OnAnyRequestError.Broadcast(ConnectionName, StatusCode);
}

void UEmergenceSingleton::GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(ResponseCode))) {
		OnGetQRCodeCompleted.Broadcast(nullptr, ResponseCode);
		OnAnyRequestError.Broadcast("GetQRCode", ResponseCode);
		return;
	}

	TArray<uint8> ResponceBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
#if UNREAL_MARKETPLACE_BUILD
		UEmergenceSingleton::DeviceID = HttpResponse->GetHeader("deviceId");
#endif
		OnGetQRCodeCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		return;
	}
	else {
		OnGetQRCodeCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientWrongType);
		OnAnyRequestError.Broadcast("GetQRCode", EErrorCode::EmergenceClientWrongType);
	}
}

void UEmergenceSingleton::GetQRCode()
{
	this->DeviceID = ""; //clear the device ID, we'll be getting a new one so we don't want to be able to accidently send an old header

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::GetQRCode_HttpRequestComplete, UHttpHelperLibrary::APIBase + "qrcode");
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetQRCode request started, calling GetQRCode_HttpRequestComplete on request completed"));
}

bool UEmergenceSingleton::RawDataToBrush(FName ResourceName, const TArray< uint8 >& InRawData, UTexture2D*& LoadedT2D)
{
	int32 Width;
	int32 Height;

	TArray<uint8> DecodedImage;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InRawData.GetData(), InRawData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!LoadedT2D) return false;

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			LoadedT2D->UpdateResource();
			return true;
		}
	}
	return false;
}

void UEmergenceSingleton::GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetStringField("address", Address)) {
			OnGetHandshakeCompleted.Broadcast(Address, StatusCode);
			this->CurrentAddress = Address;
			GetAccessToken();
		}
		else {
			OnGetHandshakeCompleted.Broadcast(Address, EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("GetHandshake", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetHandshakeCompleted.Broadcast(FString(), StatusCode);
	OnAnyRequestError.Broadcast("GetHandshake", StatusCode);
}

void UEmergenceSingleton::GetHandshake()
{
	auto ChainData = UEmergenceChain::GetEmergenceChainDataFromConfig(this);
	FString NodeURL = ChainData->NodeURL;
#if WITH_EDITOR
	UE_LOG(LogEmergenceHttp, Display, TEXT("Using chain %s, node URL: %s"), *ChainData->Name.ToString(), *NodeURL);
#endif

	TArray<TPair<FString, FString>> Headers;
	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", UEmergenceSingleton::DeviceID));
	}
	
	GetHandshakeRequest = UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(
		this,&UEmergenceSingleton::GetHandshake_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "handshake" + "?nodeUrl=" + NodeURL,
		"GET", 60.F, Headers);  //extra time because they might be fiddling with their phones
	
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetHandshake request started, calling GetHandshake_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::ReinitializeWalletConnect_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnReinitializeWalletConnectCompleted.Broadcast(StatusCode);
		return;
	}
	OnReinitializeWalletConnectCompleted.Broadcast(StatusCode);
	OnAnyRequestError.Broadcast("ReinitializeWalletConnect", StatusCode);
}

void UEmergenceSingleton::ReinitializeWalletConnect()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this, &UEmergenceSingleton::ReinitializeWalletConnect_HttpRequestComplete, UHttpHelperLibrary::APIBase + "reinitializewalletconnect");
	UE_LOG(LogEmergenceHttp, Display, TEXT("ReinitializeWalletConnect request started, calling ReinitializeWalletConnect_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool IsConnected;
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("isConnected", IsConnected)) {
			Address = JsonObject.GetObjectField("message")->GetStringField("address");
			OnIsConnectedCompleted.Broadcast(IsConnected, Address, StatusCode);
		}
		else {
			OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("IsConnected", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnIsConnectedCompleted.Broadcast(false, FString(), StatusCode);
	OnAnyRequestError.Broadcast("IsConnected", StatusCode);
}

void UEmergenceSingleton::IsConnected()
{
#if UNREAL_MARKETPLACE_BUILD
	if (UEmergenceSingleton::DeviceID.IsEmpty()) {
		OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceOk);
		return;
	}
#endif

	TArray<TPair<FString, FString>> Headers;
	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", UEmergenceSingleton::DeviceID));
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::IsConnected_HttpRequestComplete, UHttpHelperLibrary::APIBase + "isConnected", "GET", 60.0F, Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("IsConnected request started, calling IsConnected_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool Disconnected;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("disconnected", Disconnected)) {
			OnKillSessionCompleted.Broadcast(Disconnected, StatusCode);
			this->CurrentAccessToken = "";
		}
		else {
			OnKillSessionCompleted.Broadcast(Disconnected, EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("KillSession", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnKillSessionCompleted.Broadcast(false, StatusCode);
	OnAnyRequestError.Broadcast("KillSession", StatusCode);
}

void UEmergenceSingleton::KillSession()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>("Auth", this->CurrentAccessToken));
	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", UEmergenceSingleton::DeviceID));
	}
	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::KillSession_HttpRequestComplete, UHttpHelperLibrary::APIBase + "killSession", "GET", 60.0F, Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("KillSession request started, calling KillSession_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::OnOverlayClosed()
{
	auto OpeningPlayerController = CurrentEmergenceUI->GetPlayerContext().GetPlayerController();
	OpeningPlayerController->SetShowMouseCursor(this->PreviousMouseShowState);
	switch (this->PreviousGameInputMode) {
	case 0:
		OpeningPlayerController->SetInputMode(FInputModeGameAndUI());
		break;
	case 1:
		OpeningPlayerController->SetInputMode(FInputModeUIOnly());
		break;
	case 2:
		OpeningPlayerController->SetInputMode(FInputModeGameOnly());
		break;
	}
	CurrentEmergenceUI->Closed.RemoveDynamic(this, &UEmergenceSingleton::OnOverlayClosed);
}

void UEmergenceSingleton::GetAccessToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;	
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);	
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString OutputString;
		TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.GetObjectField("message").ToSharedRef(), Writer);
		OutputString.ReplaceInline(TEXT("\"accessToken\":"), TEXT(""), ESearchCase::CaseSensitive);
		OutputString.LeftChopInline(1);
		OutputString.RightChopInline(1);
		this->CurrentAccessToken = OutputString;
		UE_LOG(LogEmergenceHttp, Display, TEXT("Got access token! It is: %s"), *this->CurrentAccessToken);
		OnGetAccessTokenCompleted.Broadcast(StatusCode);
		return;
	}
	OnGetAccessTokenCompleted.Broadcast(StatusCode);
	OnAnyRequestError.Broadcast("GetAccessToken", StatusCode);
}

void UEmergenceSingleton::GetAccessToken()
{
	TArray<TPair<FString, FString>> Headers;
	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", UEmergenceSingleton::DeviceID));
	}
	GetAccessTokenRequest = UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::GetAccessToken_HttpRequestComplete, UHttpHelperLibrary::APIBase + "get-access-token", "GET", 60.0F, Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetAccessToken request started, calling GetAccessToken_HttpRequestComplete on request completed"));
}

bool UEmergenceSingleton::GetAvatarByGUIDFromCache(FString GUID, FEmergenceAvatarMetadata& FoundAvatar)
{
	for (int i = 0; i < OwnedAvatarNFTCache.Num(); i++) {
		FEmergenceAvatarMetadata* AvatarMetadata = OwnedAvatarNFTCache[i].Avatars.FindByPredicate([&](FEmergenceAvatarMetadata Avatar) {
			return Avatar.GUID == GUID;
			});
		if (AvatarMetadata) {
			FoundAvatar = *AvatarMetadata;
			return true;
		}
	}

	return false;
}
