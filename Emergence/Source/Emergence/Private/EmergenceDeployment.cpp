// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceDeployment.h"
#include "HttpService/HttpHelperLibrary.h"

UEmergenceDeployment* UEmergenceDeployment::CreateEmergenceDeployment(FString _Address, UEmergenceChain* _Blockchain, UEmergenceContract* _Contract)
{
	if (!_Blockchain || !_Contract || _Address.IsEmpty()) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("CreateEmergenceDeployment was missing a Blockchain, Contract or Address."));
		return nullptr;
	}

	UEmergenceDeployment* EmergenceDeployment = NewObject<UEmergenceDeployment>(UEmergenceDeployment::StaticClass());
	EmergenceDeployment->Address = _Address;
	EmergenceDeployment->Blockchain = _Blockchain;
	EmergenceDeployment->Contract = _Contract;
	return EmergenceDeployment;
}