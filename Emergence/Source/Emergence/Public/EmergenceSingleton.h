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
#include "Futurepass/GetLinkedFuturepassInformation.h"
#include "Environment.h"
#include "EmergenceSingleton.generated.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceSingleton();

	template<typename T>
	inline static T StringToEnum(const FString& Name) {
		UEnum* EnumClass = StaticEnum<T>();
		if (!EnumClass) {
			UE_LOG(LogTemp, Fatal, TEXT("StringToEnum Enum not found: %s"), *Name);
		}
		return (T)EnumClass->GetIndexByName(FName(*Name), EGetByNameFlags::ErrorIfNotFound);
	}

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

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	static EFutureverseEnvironment GetFutureverseEnvironment();

	//Gets the project's Overlay Login Type from the project settings
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	EmergenceLoginType GetProjectLoginType();

	//Sets the Emergence Singleton's cache of the futurepass information (and sets FuturepassInfoIsSet to true)
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void SetFuturepassInfomationCache(FLinkedFuturepassInformationResponse FuturepassInfo);

	//Clears the Emergence Singleton's cache of the futurepass information (and sets FuturepassInfoIsSet to false)
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void ClearFuturepassInfomationCache();

	//Cache of the last Futurepass information set as part of SetFuturepassInfomationCache
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Futureverse")
	FLinkedFuturepassInformationResponse FuturepassInfoCache;

	//Is FuturepassInfoCache valid?
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Futureverse")
	bool FuturepassInfoCacheIsSet = false;

	UPROPERTY()
	FString DeviceID;

	UPROPERTY(BlueprintReadOnly, Category = "Emergence")
	FEmergencePersona CachedCurrentPersona;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCachedPersonaUpdated, FEmergencePersona, NewPersona);

	//When the user's active persona changes, this is called
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
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

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	const bool IsMarketplaceBuild();

	//HTTPService Functions
private:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> GetAccessTokenRequest, GetHandshakeRequest;

	UPROPERTY()
	FString CurrentAccessToken = "";

	UPROPERTY()
	FString CurrentAddress = "";

	UPROPERTY()
	FString CurrentChecksummedAddress = "";

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

	/**
	 * Opens the Emergence UI (also known as the Overlay), returns the widget to focus.
	 * @param OwnerPlayerController Player Controller that the overlay should be shown to. Should usually be the local player, who is usually "Player Controller 0".
	 * @param EmergenceUIClass Should always be set to "EmergenceUI_BP".
	 */
	UFUNCTION(BlueprintCallable, Category = "Emergence|Overlay", meta = (DeterminesOutputType = "EmergenceUIClass"))
	UEmergenceUI* OpenEmergenceUI(APlayerController* OwnerPlayerController, TSubclassOf<UEmergenceUI> EmergenceUIClass);

	//Gets the Emergence UI (also known as the Overlay), also known as the Emergence Overlay
	UFUNCTION(BlueprintPure, Category = "Emergence|Overlay")
	UEmergenceUI* GetEmergenceUI();

	//Do we have an access token? This will likely only be true when the player has logged in via wallet connect.
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	bool HasAccessToken();

	//Do we have a wallet connected address? This will likely only be true when the player has logged in via wallet connect.
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	bool HasCachedAddress();

	//Returns the last wallet connected address (if GetHandshake has been called already) If we don't have one yet, returns "-1".
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	FString GetCachedAddress();

	UFUNCTION()
	FString GetCachedChecksummedAddress();

	//GetWalletConnectURI stuff
	UFUNCTION(BlueprintCallable, meta=(DeprecatedFunction), Category = "Emergence Internal|Overlay Methods")
	void GetWalletConnectURI();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnyRequestError, FString, ConnectionName, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAnyRequestError OnAnyRequestError;

	//This shouldn't be necessary, you should be able to call .broadcast but I couldn't get it to show up in CreatePersona for some reason
	void CallRequestError(FString ConnectionName, EErrorCode StatusCode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetQRCode();

	UFUNCTION()
	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, FString, WalletConnectString, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetHandshake(int Timeout = 60);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//ReinitializeWalletConnect stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void ReinitializeWalletConnect();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReinitializeWalletConnectCompleted, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnReinitializeWalletConnectCompleted OnReinitializeWalletConnectCompleted;

	//isConnected stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIsConnectedCompleted, bool, IsConnected, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//killSession stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void KillSession();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSessionCompleted, bool, Response, EErrorCode, StatusCode);

	//Called when the Emergence session ends and a new WalletConnect connection can be started
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnKillSessionCompleted OnKillSessionCompleted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetAccessTokenCompleted, EErrorCode, StatusCode);
	
	//Called when the user has done the last step of the login process to Emergence
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnGetAccessTokenCompleted OnGetAccessTokenCompleted;

	//This is a hacky way of logging in via an existing access token, do not use this in production. It won't work with most methods anyway, only for testing the UI stuff (won't work with walletconnect requiring stuff).
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Debug Commands")
	void ForceLoginViaAccessToken(FString AccessToken);

	//this is used by ForceLoginViaAccessToken to override the returned value of IsConnected to true, if enabled
	UPROPERTY()
	bool ForceIsConnected = false;

	UFUNCTION()
	void OnOverlayClosed();

private:
	static TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> GlobalManagers;
	TWeakObjectPtr<UGameInstance> OwningGameInstance;

	bool PreviousMouseShowState;
	int PreviousGameInputMode;
};
#pragma warning( pop )