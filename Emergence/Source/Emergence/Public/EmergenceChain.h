// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "Emergence.h"
#include "EmergenceChain.generated.h"

UENUM(BlueprintType)
enum class EEmergenceChain : uint8
{
	Ethereum,
	Polygon,
	Custom
};

UENUM(BlueprintType)
enum class EEmergenceEthereumChain : uint8
{
	MainnetInfura UMETA(DisplayName = "mainnet.infura.io (Crucible's key)"),
	RinkebyInfura UMETA(DisplayName = "rinkeby.infura.io (Crucible's key)"),
	GoerliInfura UMETA(DisplayName = "goerli.infura.io (Crucible's key)")
};

UENUM(BlueprintType)
enum class EEmergencePolygonChain : uint8
{
	MainnetInfura UMETA(DisplayName = "polygon-mainnet.infura.io (Crucible's key)"),
	MumbaiInfura UMETA(DisplayName = "polygon-mumbai.infura.io (Crucible's key)")
};

USTRUCT() struct FEmergenceChainStruct
{
	GENERATED_BODY()

	//The EVM network to connect to. Sets the token symbol (unless its overriden with a custom token symbol) and provides RPC nodes to connect to (unless overriden with a custom node).
	UPROPERTY(EditAnywhere)
	EEmergenceChain Chain = EEmergenceChain::Polygon;

	//The Etherium RPC node to connect to.
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Ethereum Node", EditConditionHides, EditCondition = "Chain==EEmergenceChain::Ethereum&&!UseCustomNodeURL"))
	EEmergenceEthereumChain EthereumChain = EEmergenceEthereumChain::MainnetInfura;

	//The Polygon RPC node to connect to.
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Polygon Node", EditConditionHides, EditCondition = "Chain==EEmergenceChain::Polygon&&!UseCustomNodeURL"))
	EEmergencePolygonChain PolygonChain = EEmergencePolygonChain::MainnetInfura;
	
	//Should we use the provided custom node URL?
	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "Chain!=EEmergenceChain::Custom"))
	bool UseCustomNodeURL = false;

	//A custom RPC node to use instead.
	UPROPERTY(meta = (EditConditionHides, DisplayName = "Custom RPC Node URL", EditCondition = "UseCustomNodeURL||Chain==EEmergenceChain::Custom"), config, EditAnywhere, Category = "Chain")
	FString CustomNodeURL;

	//Should we use the provided custom token symbol?
	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "Chain!=EEmergenceChain::Custom"))
	bool UseCustomTokenSymbol = false;

	//A custom token symbol to use instead.
	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "UseCustomTokenSymbol||Chain==EEmergenceChain::Custom", DisplayName = "Custom Token Symbol"))
	FString CustomTokenSymbol;

	void InitFromString(const FString& InSourceString)
	{
		FString ChainName, EthereumChainName, PolygonChainName;

		FParse::Value(*InSourceString, TEXT("Chain="), ChainName);
		FParse::Value(*InSourceString, TEXT("EthereumChain="), EthereumChainName);
		FParse::Value(*InSourceString, TEXT("PolygonChain="), PolygonChainName);
		FParse::Bool(*InSourceString, TEXT("UseCustomNodeURL="), UseCustomNodeURL);
		FParse::Value(*InSourceString, TEXT("CustomNodeURL="), CustomNodeURL);
		FParse::Bool(*InSourceString, TEXT("UseCustomTokenSymbol="), UseCustomTokenSymbol);
		FParse::Value(*InSourceString, TEXT("CustomTokenSymbol="), CustomTokenSymbol);

		if (ChainName == TEXT("Ethereum")) {
			Chain = EEmergenceChain::Ethereum;
		}

		if (ChainName == TEXT("Polygon")) {
			Chain = EEmergenceChain::Polygon;
		}

		if (ChainName == TEXT("Custom")) {
			Chain = EEmergenceChain::Custom;
		}

		if (EthereumChainName == TEXT("MainnetInfura")) {
			EthereumChain = EEmergenceEthereumChain::MainnetInfura;
		}

		if (EthereumChainName == TEXT("RinkebyInfura")) {
			EthereumChain = EEmergenceEthereumChain::RinkebyInfura;
		}

		if (EthereumChainName == TEXT("GoerliInfura")) {
			EthereumChain = EEmergenceEthereumChain::GoerliInfura;
		}

		if (PolygonChainName == TEXT("MainnetInfura")) {
			PolygonChain = EEmergencePolygonChain::MainnetInfura;
		}

		if (PolygonChainName == TEXT("MumbaiInfura")) {
			PolygonChain = EEmergencePolygonChain::MumbaiInfura;
		}

	}

	FString GetChainURL() {
		if (UseCustomNodeURL || Chain == EEmergenceChain::Custom) {
			return CustomNodeURL;
		}
		else {
			switch (Chain) {
			case EEmergenceChain::Polygon:
				switch (PolygonChain) {
				case EEmergencePolygonChain::MainnetInfura:
					return "https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
				case EEmergencePolygonChain::MumbaiInfura:
					return "https://polygon-mumbai.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
				}
				break;
			case EEmergenceChain::Ethereum:
				switch (EthereumChain) {
				case EEmergenceEthereumChain::MainnetInfura:
					return "https://mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
				case EEmergenceEthereumChain::RinkebyInfura:
					return "https://rinkeby.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
				case EEmergenceEthereumChain::GoerliInfura:
					return "https://goerli.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
				}
				break;
			}
		}
		return "https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
	}

	FString GetChainSymbol() {
		if (UseCustomTokenSymbol || Chain == EEmergenceChain::Custom) {
			return CustomTokenSymbol;
		}
		else {
			switch (Chain) {
			case EEmergenceChain::Polygon:
				return "MATIC";
			case EEmergenceChain::Ethereum:
				return "ETH";
			}
		}
		return CustomTokenSymbol;
	}
};


UCLASS()
class EMERGENCE_API UChainDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION()
		static FEmergenceChainStruct GetEmergenceChainDataFromConfig() {
		FString FEmergenceChainStructText;
		FEmergenceChainStruct ChainData;
		if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("Chain"), FEmergenceChainStructText, GGameIni))
		{
			ChainData.InitFromString(FEmergenceChainStructText);
		}
		else {
			UE_LOG(LogEmergenceHttp, Warning, TEXT("Couldn't get plugin settings from Game ini, returning default struct."));
		}

		return ChainData;
	};
};