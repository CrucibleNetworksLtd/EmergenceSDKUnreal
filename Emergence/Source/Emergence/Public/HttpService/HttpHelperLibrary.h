// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/EngineVersion.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceSingleton.h"
#include "PlatformHttp.h"
#include "Kismet/GameplayStatics.h"
#include "EmergenceEVMServerSubsystem.h"
#include "Engine/Engine.h"
#include "Misc/EngineVersionComparison.h"
#include "HttpHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UHttpHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static FString APIBase;

	inline static FString EmergenceVersionNumberCache;

	inline static FString GetEmergenceVersionNumber() {
		if (UHttpHelperLibrary::EmergenceVersionNumberCache.IsEmpty()) { //if the cache is empty
			IPluginManager& PluginManager = IPluginManager::Get();
			TArray<TSharedRef<IPlugin>> Plugins = PluginManager.GetDiscoveredPlugins();
			for (const TSharedRef<IPlugin>& Plugin : Plugins) {
				if (Plugin->GetName() == "Emergence") {
					const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
					UHttpHelperLibrary::EmergenceVersionNumberCache = Descriptor.VersionName;
					return UHttpHelperLibrary::EmergenceVersionNumberCache;
				}
			}
			return "error";
		}
		else { //if we have it already
			return UHttpHelperLibrary::EmergenceVersionNumberCache;
		}
	}

	inline static FString GetInventoryServiceAPIURL() {
		return "https://" + GetInventoryServiceHostURL() + "/InventoryService/";
	}

	inline static FString GetAvatarServiceAPIURL() {		
		return "https://" + GetAvatarServiceHostURL() + "/AvatarSystem/";
	}

	inline static FString GetFutureverseAssetRegistryAPIURL() {

#if UE_BUILD_SHIPPING
		FString Environment = "Production"; //Shipping defaults to production
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseShippingEnvironment"), Environment, GGameIni);
#else
		FString Environment = "Staging"; //Everything else defaults to staging
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseDevelopmentEnvironment"), Environment, GGameIni);
#endif

		if (Environment == "Production") {
			//Production Env URL
			return "https://ar-api.futureverse.app/graphql";
		}

		if (Environment == "Development") {
			//Development Env URL
			return "https://ar-api.futureverse.dev/graphql";
		}

		//Staging Env URL
		return "https://ar-api.futureverse.cloud/graphql";
	}

	inline static FString GetFutureverseFuturepassChainId() {

#if UE_BUILD_SHIPPING
		FString Environment = "Production"; //Shipping defaults to production
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseDevelopmentEnvironment"), Environment, GGameIni);
#else
		FString Environment = "Staging"; //Everything else defaults to staging
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseShippingEnvironment"), Environment, GGameIni);
#endif

		if (Environment == "Production") {
			//Production Env URL
			return "1";
		}

		//Staging & Dev Env URL
		return "11155111";
	}

	inline static FString GetFutureverseFuturepassAPIURL() {

#if UE_BUILD_SHIPPING
		FString Environment = "Production"; //Shipping defaults to production
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseDevelopmentEnvironment"), Environment, GGameIni);
#else
		FString Environment = "Staging"; //Everything else defaults to staging
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseShippingEnvironment"), Environment, GGameIni);
#endif

		if (Environment == "Production") {
			//Production Env URL
			return "https://account-indexer.pass.online/api/v1";
		}

		//Staging and Dev same Env URL
		return "https://account-indexer.passonline.dev/api/v1";
	}

	inline static FString GetPersonaAPIURL() {

#if UE_BUILD_SHIPPING
		FString DevelopmentEnvironmentString = "Production";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DevelopmentEnvironment"), DevelopmentEnvironmentString, GGameIni);
#else
		FString DevelopmentEnvironmentString = "Staging";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShippingEnvironment"), DevelopmentEnvironmentString, GGameIni);
#endif

		bool IsDevelopmentMode = false;
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("EnableDevelopmentEnvironment"), IsDevelopmentMode, GGameIni);

		if (IsDevelopmentMode) {
			return "https://57l0bi6g53.execute-api.us-east-1.amazonaws.com/staging/";
		}

		if (DevelopmentEnvironmentString == "Production") {
			return "https://i30mnhu5vg.execute-api.us-east-1.amazonaws.com/prod/";
		}

		return "https://x8iq9e5fq1.execute-api.us-east-1.amazonaws.com/staging/";
	}

	inline static FString GetInventoryServiceHostURL() {

#if UE_BUILD_SHIPPING
		FString DevelopmentEnvironmentString = "Production";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DevelopmentEnvironment"), DevelopmentEnvironmentString, GGameIni);
#else
		FString DevelopmentEnvironmentString = "Staging";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShippingEnvironment"), DevelopmentEnvironmentString, GGameIni);
#endif

		bool IsDevelopmentMode = false;
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("EnableDevelopmentEnvironment"), IsDevelopmentMode, GGameIni);

		if (IsDevelopmentMode) {
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, this is the staging URL because currently there is no dev url. This needs updating
		}

		if (DevelopmentEnvironmentString == "Production") {
			return "7vz9y7rdpy.us-east-1.awsapprunner.com";
		}

		return "dysaw5zhak.us-east-1.awsapprunner.com";
	}

	inline static FString GetAvatarServiceHostURL() {

#if UE_BUILD_SHIPPING
		FString DevelopmentEnvironmentString = "Production";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DevelopmentEnvironment"), DevelopmentEnvironmentString, GGameIni);
#else
		FString DevelopmentEnvironmentString = "Staging";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShippingEnvironment"), DevelopmentEnvironmentString, GGameIni);
#endif

		bool IsDevelopmentMode = false;
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("EnableDevelopmentEnvironment"), IsDevelopmentMode, GGameIni);

		if (IsDevelopmentMode) {
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, this is the staging URL because currently there is no dev url. This needs updating
		}

		if (DevelopmentEnvironmentString == "Production") {
//			return "7vz9y7rdpy.us-east-1.awsapprunner.com";
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, only staging is used right now
		}

		return "dysaw5zhak.us-east-1.awsapprunner.com";
	}

	inline static FString InternalIPFSURLToHTTP(FString IPFSURL) {

		if (IPFSURL.Contains(TEXT("ipfs://")) || IPFSURL.Contains(TEXT("IPFS://")) || IPFSURL.Contains(TEXT("https://cloudflare-ipfs.com/ipfs/"))) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Found %s URL, replacing with public node..."), IPFSURL.Contains(TEXT("https://cloudflare-ipfs.com/ipfs/")) ? TEXT("Cloudflare-IPFS") : TEXT("IPFS"));

			FString IPFSNode = TEXT("http://ipfs.openmeta.xyz/ipfs/");
			FString CustomIPFSNode = "";
			if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("IPFSNode"), CustomIPFSNode, GGameIni))
			{
				if (CustomIPFSNode != "") {
					UE_LOG(LogEmergenceHttp, Display, TEXT("Found custom IPFS node in game config, replacing with \"%s\""), *CustomIPFSNode);
					IPFSNode = CustomIPFSNode;
				}
			}
			FString NewURL = IPFSURL.Replace(TEXT("ipfs://"), *IPFSNode)
				.Replace(TEXT("IPFS://"), *IPFSNode)
				.Replace(TEXT("https://cloudflare-ipfs.com/ipfs/"), *IPFSNode);
			UE_LOG(LogEmergenceHttp, Display, TEXT("New URL is \"%s\""), *NewURL);
			return NewURL;
		}
		else {
			return IPFSURL;
		}
	}

	/**
	 * Takes an IPFS URL and changes it to be a IPFS gateway link (IPFS via HTTP). If its already HTTP, no conversion happens.
	 * @param IPFSURL The URL to try to convert. If its already HTTP, no conversion happens.
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Helpers")
	static FString IPFSURLToHTTP(FString IPFSURL) {
		return UHttpHelperLibrary::InternalIPFSURLToHTTP(IPFSURL);
	}

	template<typename T>
	inline static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ExecuteHttpRequest(T* FunctionBindObject, void(T::* FunctionBindFunction)(FHttpRequestPtr, FHttpResponsePtr, bool), const FString& URL, const FString& Verb = TEXT("GET"), const float& Timeout = 60.0F, const TArray<TPair<FString, FString>>& Headers = TArray<TPair<FString, FString>>(), const FString& Content = FString(), const bool ProcessRequestInstantly = true)
	{
		static_assert(std::is_base_of<UObject, T>::value, "T not derived from UObject");

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		if (GEngine && static_cast<UObject*>(FunctionBindObject)) {
			UObject* WorldContextObject = static_cast<UObject*>(FunctionBindObject);
			if (WorldContextObject) {
				UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
				if (
					World && //get the world
					World->GetGameInstance()) { //if we actually got a world, get the game instance
					UEmergenceEVMServerSubsystem* EmergenceSubsystem = World->GetGameInstance()->GetSubsystem<UEmergenceEVMServerSubsystem>();
					if (EmergenceSubsystem) {
						EmergenceSubsystem->ActiveRequests.Add(HttpRequest);
					}
				};
			}
		}

		if (FunctionBindFunction && FunctionBindObject) {
			HttpRequest->OnProcessRequestComplete().BindUObject(FunctionBindObject, FunctionBindFunction);
		}

		if (URL.IsEmpty()) {
			UE_LOG(LogEmergenceHttp, Warning, TEXT("Tried to ExecuteHttpRequest but URL was empty"));
			HttpRequest->CancelRequest();
			return HttpRequest;
		}

		FString FinalURL;

		//switch IPFS to our public node...
		FinalURL = UHttpHelperLibrary::InternalIPFSURLToHTTP(URL);
		HttpRequest->SetURL(FinalURL);
		HttpRequest->SetVerb(Verb);
		HttpRequest->SetTimeout(Timeout);
#if UE_VERSION_NEWER_THAN(5, 4, 0)
		HttpRequest->SetActivityTimeout(Timeout);
#endif
		//Handle headers and logging of the headers
		FString HeaderLogText;
		if (Headers.Num() > 0) {
			HeaderLogText = "\nAdditional headers:\n";
			for (int i = 0; i < Headers.Num(); i++) {
				HttpRequest->SetHeader(Headers[i].Key, Headers[i].Value);
				HeaderLogText.Append(Headers[i].Key + ": " + Headers[i].Value + "\n");
			}
		}
#if UNREAL_MARKETPLACE_BUILD
		FString Version = "Emergence " + GetEmergenceVersionNumber() + " EVMOnline";
#else
		FString Version = "Emergence " + GetEmergenceVersionNumber() + " LocalEVM";
#endif
		HttpRequest->SetHeader("User-Agent", FPlatformHttp::GetDefaultUserAgent() + " " + Version);

		if (Content.Len() > 0 && HttpRequest->GetHeader("Content-Type").Len() > 0) {
			HttpRequest->SetContentAsString(Content);
		}

		if (ProcessRequestInstantly) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Sent %s request to \"%s\", timing out in %f %s \n%s"), *Verb, *FinalURL, Timeout, *HeaderLogText, *Content);
			HttpRequest->ProcessRequest();
		}
		else {
			UE_LOG(LogEmergenceHttp, Display, TEXT("%s request ready to send to \"%s\", will time out in %f %s \n%s"), *Verb, *FinalURL, Timeout, *HeaderLogText, *Content);
		}

		return HttpRequest;
	};
};
