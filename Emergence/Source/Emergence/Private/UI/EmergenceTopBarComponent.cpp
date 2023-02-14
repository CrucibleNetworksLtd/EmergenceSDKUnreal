// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "UI/EmergenceTopBarComponent.h"
#include "WalletService/GetBalance.h"
#include "EmergenceSingleton.h"
#include "UnitConverterFunctionLibrary.h"
#include "WalletService/ReadMethod.h"
#include "WalletService/ContractInterfaces/ERC20Contract.h"
#include "WalletService/EmergenceJSONHelpers.h"

#define LOCTEXT_NAMESPACE "TopBarNamespace"

void UEmergenceTopBarComponent::StartGetBalanceTextAsync() {

	CurrencyDisplayText = ""; //Clear the currency display text

	//We need both of these no matter if we're using an ERC20 or not
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(this);
	this->CurrencyDisplayChainData = UEmergenceChain::GetEmergenceChainDataFromConfig(this);

	//figure out if we want to use an ERC20 as the balance
	bool UseERC20AsBalance;
	if (!GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("UseERC20AsBalance"), UseERC20AsBalance, GGameIni)) {
		UseERC20AsBalance = false; //if we failed to get the config, default to false
	}

	FString ERC20Address;
	if (UseERC20AsBalance) {
		if (!GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ERC20Address"), ERC20Address, GGameIni)) {
			ERC20Address = "";
		}
	}

	//if we don't want to use ERC20 as the balance or it isn't set up correctly
	if (!UseERC20AsBalance || ERC20Address.IsEmpty()) {
		this->ReturnedSymbol = CurrencyDisplayChainData->Symbol; //set the symbol from the CurrencyDisplayChainData
		//and call GetBalance
		UGetBalance* GetBalanceRequest = UGetBalance::GetBalance(this, Singleton->GetCachedAddress(), CurrencyDisplayChainData);
		GetBalanceRequest->OnGetBalanceCompleted.AddDynamic(this, &UEmergenceTopBarComponent::GetBalanceResponseHandler);
		GetBalanceRequest->Activate();
	}
	else { //if we are using an ERC20 as the balance
		//Create deployed contract for this
		UEmergenceDeployment* ERC20Deployment = NewObject<UEmergenceDeployment>();
		FText CustomBlockchainName = FText::FromString(this->CurrencyDisplayChainData->Symbol + "FOR_UI");
		ERC20Deployment->Blockchain = UEmergenceChain::CreateEmergenceChain(
			CustomBlockchainName,
			this->CurrencyDisplayChainData->NodeURL,
			this->CurrencyDisplayChainData->Symbol,
			this->CurrencyDisplayChainData->ChainID);
		ERC20Deployment->Address = ERC20Address;
		ERC20Deployment->Contract = NewObject<UERC20Contract>();
		UReadMethod* BalanceOfRequest =  UReadMethod::ReadMethod(this, ERC20Deployment, FEmergenceContractMethod("balanceOf"), { Singleton->GetCachedAddress() });
		BalanceOfRequest->OnReadMethodCompleted.AddDynamic(this, &UEmergenceTopBarComponent::BalanceOfResponseHandler);
		BalanceOfRequest->Activate();
		UReadMethod* SymbolRequest = UReadMethod::ReadMethod(this, ERC20Deployment, FEmergenceContractMethod("symbol"), {});
		SymbolRequest->OnReadMethodCompleted.AddDynamic(this, &UEmergenceTopBarComponent::SymbolResponseHandler);
		SymbolRequest->Activate();
	}
}

bool UEmergenceTopBarComponent::ShouldDisplayBalanceText()
{
	bool ShouldShowBalance;
	if (!GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShowBalance"), ShouldShowBalance, GGameIni)) {
		ShouldShowBalance = true; //if we failed to get the config, default to true
	}
	return ShouldShowBalance;
}

void UEmergenceTopBarComponent::GetBalanceResponseHandler(FString Balance, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetBalanceResponseHandler came back with %s"), *Balance);
		if (Balance.IsNumeric()) {
			FCString::Atoi(*Balance); //this will come back 0 if it fails, like if its all text
			this->ReturnedBalance = Balance;
			CurrencyDisplayText = GetBalanceText();
			BalanceTextUpdated();
		}
		else {
			CurrencyDisplayText = "COULDNT CONV";
		}
	}
	else {
		CurrencyDisplayText = "ERM ERROR";
	}
}

void UEmergenceTopBarComponent::BalanceOfResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		TArray<FString> StringArray;
		if (UEmergenceJSONHelpers::ReadMethodJSONToStringArray(Response, StringArray)) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("BalanceOfResponseHandler came back with %s"), *StringArray[0]);
			this->ReturnedBalance = StringArray[0];
			CurrencyDisplayText = GetBalanceText();
			BalanceTextUpdated();
		}
		else {
			CurrencyDisplayText = "JASTS ERROR";
		}
	}
	else {
		CurrencyDisplayText = "ERM ERROR";
	}
}

void UEmergenceTopBarComponent::SymbolResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		TArray<FString> StringArray;
		if (UEmergenceJSONHelpers::ReadMethodJSONToStringArray(Response, StringArray)) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("SymbolResponseHandler StringArray[0] %s"), *StringArray[0]);
			this->ReturnedSymbol = StringArray[0];
		}
		BalanceTextUpdated();
	}
	else {
		this->ReturnedSymbol = "SYM ERROR";
	}
}

FString UEmergenceTopBarComponent::GetBalanceText()
{
	if (this->ReturnedSymbol.IsEmpty()) {
		return "";
	}

	if (this->ReturnedBalance == "0") {
		return "0 " + this->ReturnedSymbol;
	}
	FString IntergerSide, DecimalSide;
	FString AsEther = UUnitConverterFunctionLibrary::Convert(this->ReturnedBalance, EEtherUnitType::WEI, EEtherUnitType::ETHER, ".");
	if (AsEther.Contains(".")) {
		AsEther.Split(".", &IntergerSide, &DecimalSide);
		return IntergerSide + "." + DecimalSide.Left(3) + " " + this->ReturnedSymbol;
	}
	else {
		return AsEther + " " + this->ReturnedSymbol;
	}
}

#undef LOCTEXT_NAMESPACE