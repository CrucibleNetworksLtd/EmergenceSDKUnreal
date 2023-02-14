// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/LoadContract.h"
#include "EmergenceSingleton.h"

ULoadContract* ULoadContract::LoadContract(UObject* WorldContextObject, FString ContractAddress, FString ABI)
{
	ULoadContract* BlueprintNode = NewObject<ULoadContract>();
	return BlueprintNode;
}

void ULoadContract::Activate()
{
	UE_LOG(LogEmergenceHttp, Error, TEXT("Calling LoadContract from blueprints is now deprecated. Please remove any code that calls it ASAP!"));
	OnLoadContractCompleted.Broadcast("", EErrorCode::EmergenceOk);
}
