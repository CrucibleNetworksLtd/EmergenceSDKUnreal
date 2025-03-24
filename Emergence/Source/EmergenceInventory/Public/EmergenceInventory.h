// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"

class FEmergenceInventoryModule : public IModuleInterface
{
public:

	//our inventory service depending on environment
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
	};

	//our avatar service depending on environment
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
	};

	inline static FString GetInventoryServiceAPIURL() {
		return "https://" + GetInventoryServiceHostURL() + "/InventoryService/";
	}

	inline static FString GetAvatarServiceAPIURL() {
		return "https://" + GetAvatarServiceHostURL() + "/AvatarSystem/";
	};
};
