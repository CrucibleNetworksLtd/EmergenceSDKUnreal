// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceDeployment.h"

UEmergenceDeployment* UEmergenceDeployment::CreateEmergenceDeployment(FString _Address, UEmergenceChain* _Blockchain, UEmergenceContract* _Contract)
{
	UEmergenceDeployment* EmergenceDeployment = NewObject<UEmergenceDeployment>(UEmergenceDeployment::StaticClass());
	EmergenceDeployment->Address = _Address;
	EmergenceDeployment->Blockchain = _Blockchain;
	EmergenceDeployment->Contract = _Contract;
	return EmergenceDeployment;
}