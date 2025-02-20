// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "Types/Environment.h"
#include "Types/EmergenceChain.h"
#include "EmergencePluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UEmergencePluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer);

	//Which cloud environment should be communicated with when the game is built as "Debug", "Development" or "Test". Default is "Staging".
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (EditCondition = "!EnableDevelopmentEnvironment", DisplayName = "Debug/Development/Test build cloud environment"))
	EEmergenceEnvironment DevelopmentEnvironment = EEmergenceEnvironment::Staging;
	
	//Which cloud environment should be communicated with when the game is built as "Shipping". Default is "Production".
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (EditCondition = "!EnableDevelopmentEnvironment", DisplayName = "Shipping build cloud environment"))
	EEmergenceEnvironment ShippingEnvironment = EEmergenceEnvironment::Production;

	//Which Futureverse cloud environment should be communicated with when the game is built as "Debug", "Development" or "Test". Default is "Staging".
	UPROPERTY(config, EditAnywhere, Category = "Futureverse", meta = (DisplayName = "Debug/Development/Test build Futureverse cloud environment"))
	EFutureverseEnvironment FutureverseDevelopmentEnvironment = EFutureverseEnvironment::Production;

	//Which Futureverse cloud environment should be communicated with when the game is built as "Shipping". Default is "Production".
	UPROPERTY(config, EditAnywhere, Category = "Futureverse", meta = (DisplayName = "Shipping build Futureverse cloud environment"))
	EFutureverseEnvironment FutureverseShippingEnvironment = EFutureverseEnvironment::Production;

	//A custom client ID to use with the Futureverse web login flow on the Futureverse Production Environment. One of these can be created at https://login.futureverse.app/manageclients
	UPROPERTY(config, EditAnywhere, Category = "Futureverse")
	FString FutureverseWebLoginProductionEnvClientID = "";

	//A custom client ID to use with the Futureverse web login flow on the Futureverse Development / Staging Environments. One of these can be created at https://login.futureverse.cloud/manageclients
	UPROPERTY(config, EditAnywhere, Category = "Futureverse")
	FString FutureverseWebLoginStagingEnvClientID = "";
	
	//For Crucible use only! Enable the development environment. Not to be confused with the development build of the game.
	UPROPERTY(AdvancedDisplay, config, EditAnywhere, Category = "General", meta = (DisplayName = "[INTERNAL] Enable Development Environment"))
	bool EnableDevelopmentEnvironment = false;

	//The IPFS node to use when getting IPFS data via HTTP. Leaving it blank will use the default "http://ipfs.openmeta.xyz/ipfs/". The IPFS hash will be added to the end (for example, using the default: "http://ipfs.openmeta.xyz/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu")
	UPROPERTY(config, EditAnywhere, Category = "IPFS", meta = (DisplayName = "Custom IPFS Node"))
	FString IPFSNode;

	//If false, libcurl will be prevented from reusing connections. This can speed up requests on some systems. If you toggle this, you may have to restart the editor for it to fully have an effect.
	UPROPERTY(config, EditAnywhere, Category = "libCurl", meta = (DisplayName = "Allow LibCurl Connection Reuse", EditCondition = "EngineHasConnectionReuseConfig", EditConditionHides, HideEditConditionToggle))
	bool AllowLibcurlConnectionReuse = false;

	UPROPERTY()
	bool EngineHasConnectionReuseConfig = true;
};