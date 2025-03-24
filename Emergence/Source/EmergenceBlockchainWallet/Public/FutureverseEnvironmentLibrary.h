// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/ConfigCacheIni.h"
#include "FutureverseEnvironmentLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UFutureverseEnvironmentLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//The URL to send the user to to create a futurepass depending on environment
	UFUNCTION(BlueprintPure, Category = "Emergence|Futureverse|Futurepass")
	static FString GetFutureverseCreateFuturepassURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://futurepass.futureverse.app/";
		}

		//Staging Env URL
		return "https://identity-dashboard.futureverse.cloud/";
	};
	
	inline static FString GetFVEnvironment() {
#if UE_BUILD_SHIPPING
		FString Environment = "Production"; //Shipping defaults to production
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseShippingEnvironment"), Environment, GGameIni);
#else
		FString Environment = "Production"; //Everything else defaults to Production as well
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseDevelopmentEnvironment"), Environment, GGameIni);
#endif
		return Environment;
	};

	//Our service for helping sending custodial transactions
	inline static FString GetFVHelperServiceURL() {
		return "https://fvhelperservice.openmeta.xyz/";
	}

	//the chain id to use with futurepasses depending on environment
	inline static FString GetFutureverseFuturepassChainId() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "1";
		}

		//Staging & Dev Env URL
		return "11155111";
	};

	//the location of the futurepass API  depending on environment
	inline static FString GetFutureverseFuturepassAPIURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://account-indexer.pass.online/api/v1";
		}

		//Staging and Dev same Env URL
		return "https://account-indexer.passonline.dev/api/v1";
	};

	//the futureverse signer url depending on environment
	inline static FString GetFutureverseSignerURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			return TEXT("https://signer.pass.online");
		}
		else {
			return TEXT("https://signer.passonline.cloud");
		}
	};

	//the futureverse auth url depending on environment
	inline static FString GetFutureverseAuthURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			return TEXT("https://login.pass.online");
		}
		else {
			return TEXT("https://login.passonline.cloud");
		}
	};
};
