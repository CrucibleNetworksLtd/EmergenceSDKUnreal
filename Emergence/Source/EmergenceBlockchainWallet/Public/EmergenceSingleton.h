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
#include "Types/EmergenceErrorCode.h"
#include "GameFramework/PlayerController.h"
#include "EmergenceCore.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Types/Environment.h"
#include "WebLogin/CustodialLogin.h"
#include "WalletService/RequestToSign.h"
#include "EmergenceSingleton.generated.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS(Category = "Emergence|Emergence Singleton")
class EMERGENCEBLOCKCHAINWALLET_API UEmergenceSingleton : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//Get the global Emergence service
	UFUNCTION(BlueprintPure, Category = "Emergence", meta = (DeprecatedFunction, DeprecationMessage = "This function is deprecated. Please use the subsystem node.", DisplayName = "Get Emergence Service", WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION()
	void CompleteLoginViaWebLoginFlow(const FEmergenceCustodialLoginOutput LoginData, EErrorCode ErrorCode);

	//The device ID, used by CloudEVM to know which WalletConnect session this client is talking to
	UPROPERTY()
	FString DeviceID;

	//Stores a list of contracts with loaded ABIs, in the format blockchain name + address. 
	//This is used by various methods to check if the ABI needs to be sent to the server (if it is "new" to the server for this user)
	UPROPERTY()
	TArray<FString> ContractsWithLoadedABIs;

private:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> GetHandshakeRequest;

	void GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
public:
	//Cancels any open GetHandshake requests.
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Singleton|WalletConnect Methods")
	void CancelSignInRequest();

	//GetQRCode stuff
	UFUNCTION()
	void GetQRCode();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, FString, WalletConnectString, EErrorCode, StatusCode);

	UPROPERTY()
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION()
	void GetHandshake(int Timeout = 60);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, EErrorCode, StatusCode);

	UPROPERTY()
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//Checks if a walletconnect session is still valid.
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Singleton|WalletConnect Methods")
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIsConnectedCompleted, bool, IsConnected, FString, Address, EErrorCode, StatusCode);


	//Called when IsConnected is returned.
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton|WalletConnect Methods")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//Kills the walletconnect session. Setting TrackRequest to false will mean OnSessionEnded will never fire,
	//and this request won't be added to ActiveRequests (good to prevent this getting premptively killed going from PIE back to Editor.
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Singleton|WalletConnect Methods")
	void KillWalletConnectSession(bool TrackRequest = true);


/*
* ---- The section below are all generic methods to both Futureverse Custodial and WalletConnect style logins
*/

	//Returns the last wallet connected address. If we don't have one yet, returns "" (empty string).
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	FString GetCachedAddress(bool Checksummed = false);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionEnded, bool, Response, EErrorCode, StatusCode);

	//Called when a WalletConnect / Futureverse Custodial session ends.
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnSessionEnded OnSessionEnded;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFinished, EErrorCode, StatusCode);
	
	//Called when the user has done the last step of a login process (either WalletConnect / Futureverse Custodial), or the process has had an error that causes it to finish.
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnLoginFinished OnLoginFinished;

	//Are we logged in via a web login flow, rather than WalletConnect? 
	//This is referenced by functions such as RequestToSign and WriteMethod to allow for behaviour switching depending on login type.
	//You shouldn't need to ever change this manually. If you do, what caused you to need to change it manually is a bug and should be reported to Crucible.
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Login Flow")
	bool UsingWebLoginFlow = false;

private:
	UPROPERTY()
	FString CurrentAddress = "";

	UPROPERTY()
	FString CurrentChecksummedAddress = "";
};
#pragma warning( pop )