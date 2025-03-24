// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Types/EmergenceDeployment.h"
#include "WalletService/WriteMethod.h"

class UWriteMethod;
struct EmergenceLocalEVMJSON;

class FEmergenceBlockchainWalletModule : public IModuleInterface
{
public:

	void SendTransactionViaKeystore(UWriteMethod* WriteMethod, UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse);
private:
	/** Handle to the test dll we will load */
	void* ContainerLibraryHandle;

	typedef int(*_getSendTransactionViaKeystoreFunctionHandle)(wchar_t* fullpath, int length, EmergenceLocalEVMJSON* Data);
	_getSendTransactionViaKeystoreFunctionHandle SendTransactionViaKeystoreFunctionHandle;
};
