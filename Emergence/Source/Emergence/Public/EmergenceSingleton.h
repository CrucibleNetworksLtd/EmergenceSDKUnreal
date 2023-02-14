// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/GameInstance.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Containers/Map.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Containers/Queue.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "UI/EmergenceUI.h"
#include "GameFramework/PlayerController.h"
#include "Emergence.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "EmergenceSingleton.generated.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceSingleton();

	/** Get the global Emergence service */
	UFUNCTION(BlueprintPure, Category = "Emergence", meta = (DisplayName = "Get Emergence Service", WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods", meta = (WorldContext = "ContextObject"))
	static UEmergenceSingleton* ForceInitialize(const UObject* ContextObject);

	void Init();
	void Shutdown();

	void SetGameInstance(UGameInstance* GameInstance) { OwningGameInstance = GameInstance; }

	void SetCachedCurrentPersona(FEmergencePersona NewCachedCurrentPersona);

	inline static FString DeviceID;

	UPROPERTY(BlueprintReadOnly, Category = "Emergence")
	FEmergencePersona CachedCurrentPersona;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCachedPersonaUpdated, FEmergencePersona, NewPersona);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnCachedPersonaUpdated OnCachedPersonaUpdated;

	UPROPERTY()
	TArray<FString> ContractsWithLoadedABIs;

	UPROPERTY()
	TMap<FString, UTexture2D*> DownloadedImageCache;

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void SetOwnedAvatarNFTCache(TArray<FEmergenceAvatarResult> Results);

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void FlushOwnedAvatarNFTCache();

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	bool GetAvatarByGUIDFromCache(FString GUID, FEmergenceAvatarMetadata& FoundAvatar);

	UPROPERTY(BlueprintReadOnly, Category = "Emergence Internal|Overlay Methods")
	TArray<FEmergenceAvatarResult> OwnedAvatarNFTCache;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwnedAvatarNFTCacheUpdated);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Overlay Methods")
	FOnOwnedAvatarNFTCacheUpdated OnOwnedAvatarNFTCacheUpdated;

	UPROPERTY(BlueprintReadOnly, Category = "Emergence Internal|Overlay Methods")
	bool OwnedAvatarNFTCached = false;

	//HTTPService Functions
private:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> GetAccessTokenRequest, GetHandshakeRequest;

	UPROPERTY()
	FString CurrentAccessToken = "";

	UPROPERTY()
	FString CurrentAddress = "";

	//Returns true if this error code is a 401, and calls OnDatabaseAuthFailed. false on success.
	bool HandleDatabaseServerAuthFail(EErrorCode ErrorCode);

	void GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetAccessToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void ReinitializeWalletConnect_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetAccessToken();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDatabaseAuthFailed);
	FOnDatabaseAuthFailed OnDatabaseAuthFailed;

	UEmergenceUI* CurrentEmergenceUI;
public:
	//Cancels any open GetAccessToken and GetHandshake requests.
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Emergence Singleton")
	void CancelSignInRequest();

	//Returns the last access token. Consider calling "HasAcessToken" before you call this. If we don't have an access token yet, returns "-1".
	UFUNCTION(Category = "Emergence|Emergence Singleton", BlueprintPure, Meta = (DisplayName="Get Cached Access Token"))
	FString GetCurrentAccessToken();

	//Opens the Emergence UI, returns the widget to focus
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Singleton")
	UEmergenceUI* OpenEmergenceUI(APlayerController* OwnerPlayerController, TSubclassOf<UEmergenceUI> EmergenceUIClass);

	//Gets the Emergence UI
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	UEmergenceUI* GetEmergenceUI();

	//Do we have an access token?
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	bool HasAccessToken();

	//Do we have a wallet connected address?
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	bool HasCachedAddress();

	//Returns the last wallet connected address (if GetHandshake has been called already) If we don't have one yet, returns "-1".
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	FString GetCachedAddress();

	//GetWalletConnectURI stuff
	UFUNCTION(BlueprintCallable, meta=(DeprecatedFunction), Category = "Emergence Internal|Overlay Methods")
	void GetWalletConnectURI();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnyRequestError, FString, ConnectionName, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAnyRequestError OnAnyRequestError;

	//This shouldn't be necessary, you should be able to call .broadcast but I couldn't get it to show up in CreatePersona for some reason
	void CallRequestError(FString ConnectionName, EErrorCode StatusCode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetQRCode();

	bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetHandshake();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//ReinitializeWalletConnect stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void ReinitializeWalletConnect();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReinitializeWalletConnectCompleted, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnReinitializeWalletConnectCompleted OnReinitializeWalletConnectCompleted;

	//isConnected stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIsConnectedCompleted, bool, IsConnected, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//killSession stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void KillSession();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSessionCompleted, bool, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnKillSessionCompleted OnKillSessionCompleted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetAccessTokenCompleted, EErrorCode, StatusCode);
	
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetAccessTokenCompleted OnGetAccessTokenCompleted;

	UFUNCTION()
	void OnOverlayClosed();

private:
	static TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> GlobalManagers;
	TWeakObjectPtr<UGameInstance> OwningGameInstance;

	bool PreviousMouseShowState;
	int PreviousGameInputMode;
};
#pragma warning( pop )