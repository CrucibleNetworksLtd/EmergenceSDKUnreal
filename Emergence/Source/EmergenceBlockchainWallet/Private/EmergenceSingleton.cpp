// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceSingleton.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "GameDelegates.h"

//for HTTP services
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


#include "Dom/JsonObject.h"

#include "HttpService/HttpHelperLibrary.h"
#include "Containers/UnrealString.h"

#include "Types/EmergenceChain.h"

#include "Engine/GameViewportClient.h"

#include "ImageHelperLibrary.h"


void UEmergenceSingleton::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
}

void UEmergenceSingleton::Deinitialize()
{
	this->KillWalletConnectSession(false); //kill the session on game end. We don't need to wait to see if it happens or not, we're just trying to help out the server.
	UGameInstanceSubsystem::Deinitialize();
}

UEmergenceSingleton* UEmergenceSingleton::GetEmergenceManager(const UObject* ContextObject)
{
	if (!GEngine || !ContextObject) {
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UEmergenceSingleton>();
	}
	UE_LOG(LogEmergenceHttp, Error, TEXT("Emergence singleton error: no game instance"));
	return nullptr;
}


void UEmergenceSingleton::CompleteLoginViaWebLoginFlow(const FEmergenceCustodialLoginOutput LoginData, EErrorCode ErrorCode)
{
	if (ErrorCode == EErrorCode::EmergenceOk) {
		FString Address = LoginData.TokenData.FindRef(TEXT("eoa"));
		this->CurrentChecksummedAddress = Address;
		this->CurrentAddress = Address.ToLower();
		this->UsingWebLoginFlow = true;
		OnLoginFinished.Broadcast(EErrorCode::EmergenceOk);
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("CompleteLoginViaWebLoginFlow failed with code: %d"), (int)ErrorCode);
	}
}

void UEmergenceSingleton::CancelSignInRequest()
{
	if (GetHandshakeRequest && GetHandshakeRequest->GetStatus() == EHttpRequestStatus::Processing) {
		GetHandshakeRequest->CancelRequest();
	}
}

FString UEmergenceSingleton::GetCachedAddress(bool Checksummed)
{
	if (Checksummed) {
		if (this->CurrentChecksummedAddress.Len() > 0) {
			return this->CurrentChecksummedAddress;		
		}
		else {
			return "";
		}
	}
	else {
		if (this->CurrentAddress.Len() > 0) {
			return this->CurrentAddress;
		}
		else {
			return "";
		}
	}
}

void UEmergenceSingleton::GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UHttpHelperLibrary::GetResponseErrors(HttpRequest, HttpResponse, bSucceeded);
	if (ResponseCode != EErrorCode::Ok) { //this endpoint responds Http Ok rather than emergence ok, because its an image rather than emergence status json
		OnGetQRCodeCompleted.Broadcast(nullptr, FString(), ResponseCode);
		return;
	}

	TArray<uint8> ResponseBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (UImageHelperLibrary::RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponseBytes, QRCodeTexture)) {

		this->DeviceID = HttpResponse->GetHeader("deviceId");
		FString WalletConnectString = HttpResponse->GetHeader("walletconnecturi");
		OnGetQRCodeCompleted.Broadcast(QRCodeTexture, WalletConnectString, EErrorCode::EmergenceOk);
		return;
	}
	else {
		OnGetQRCodeCompleted.Broadcast(nullptr, FString(), EErrorCode::EmergenceClientFailed);
	}
}

void UEmergenceSingleton::GetQRCode()
{
	this->DeviceID = ""; //clear the device ID, we'll be getting a new one so we don't want to be able to accidently send an old header

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::GetQRCode_HttpRequestComplete, UHttpHelperLibrary::APIBase + "qrcode");
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetQRCode request started, calling GetQRCode_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetStringField("address", Address)) {
			OnGetHandshakeCompleted.Broadcast(Address, StatusCode);
			this->CurrentAddress = Address;
			FString ChecksummedAddress;
			if (JsonObject.GetObjectField("message")->TryGetStringField("checksummedAddress", ChecksummedAddress)) {
				this->CurrentChecksummedAddress = ChecksummedAddress;
			}
			OnLoginFinished.Broadcast(EErrorCode::EmergenceOk);
		}
		else {
			OnGetHandshakeCompleted.Broadcast(Address, EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetHandshakeCompleted.Broadcast(FString(), StatusCode);
}

void UEmergenceSingleton::GetHandshake(int Timeout)
{
	TArray<TPair<FString, FString>> Headers;
	if (!this->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));
	}
	Headers.Add(TPair<FString, FString>("timeout", FString::FromInt(Timeout + 2))); //ask for the desired timeout plus buffer time to load the next one
	
	GetHandshakeRequest = UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(
		this,&UEmergenceSingleton::GetHandshake_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "handshake",
		"GET", Timeout, Headers); //use the timeout provided
	
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetHandshake request started, calling GetHandshake_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool IsConnected;
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("isConnected", IsConnected)) {
			if (IsConnected) {
				Address = JsonObject.GetObjectField("message")->GetStringField("address");
				OnIsConnectedCompleted.Broadcast(IsConnected, Address, StatusCode);
			}
			else {
				OnIsConnectedCompleted.Broadcast(IsConnected, "", StatusCode);
			}
		}
		else {
			OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnIsConnectedCompleted.Broadcast(false, FString(), StatusCode);
}

void UEmergenceSingleton::IsConnected()
{
	if (this->DeviceID.IsEmpty()) {
		OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceOk);
		return;
	}

	TArray<TPair<FString, FString>> Headers;
	if (!this->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::IsConnected_HttpRequestComplete, UHttpHelperLibrary::APIBase + "isConnected", "GET", 60.0F, Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("IsConnected request started, calling IsConnected_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool Disconnected;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("disconnected", Disconnected)) {
			OnSessionEnded.Broadcast(Disconnected, StatusCode);
			this->DeviceID = "";
		}
		else {
			OnSessionEnded.Broadcast(Disconnected, EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnSessionEnded.Broadcast(false, StatusCode);
}

void UEmergenceSingleton::KillWalletConnectSession(bool TrackRequest)
{
	if(this->UsingWebLoginFlow){
		this->CurrentAddress = "";
		this->CurrentChecksummedAddress = "";
		this->DeviceID = "";
		
		OnSessionEnded.Broadcast(true, EErrorCode::EmergenceOk);
	
		return;
	}
	else {

		TArray<TPair<FString, FString>> Headers;

		if (this->DeviceID.IsEmpty()) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Tried to KillSession but there is no device ID, so probably no session."));
			return;
		}

		//we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));
		if (TrackRequest) {
			UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this, &UEmergenceSingleton::KillSession_HttpRequestComplete, UHttpHelperLibrary::APIBase + "killSession", "GET", 60.0F, Headers);
		}
		else {
			UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(nullptr, nullptr, UHttpHelperLibrary::APIBase + "killSession", "GET", 60.0F, Headers);
		}
		UE_LOG(LogEmergenceHttp, Display, TEXT("KillSession request started, calling KillSession_HttpRequestComplete on request completed"));
	}
}
