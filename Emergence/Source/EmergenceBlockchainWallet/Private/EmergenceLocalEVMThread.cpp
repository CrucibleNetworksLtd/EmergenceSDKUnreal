#include "EmergenceLocalEVMThread.h"
#include "Runtime/Launch/Resources/Version.h"
#include "EmergenceCore.h"

bool FLocalEVMThreadRunnable::Init()
{
	return true;
}

uint32 FLocalEVMThreadRunnable::Run()
{
	this->ExampleLibraryFunction(fullpath, length, Data);
	return 0;
}

void FLocalEVMThreadRunnable::Exit()
{
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
	UE_LOG(LogEmergence, Display, TEXT("Got transaction JSON %hs"), (char*)(Data->result));
#else
	UE_LOG(LogEmergence, Display, TEXT("Got transaction JSON %s"), (char*)(Data->result));
#endif
	FString TransactionResponse = "";
	for (int i = 0; i < (int)Data->ResultLength * 2; i++) {
		TransactionResponse.AppendChar(((char*)(Data->result))[i]);
	}
	WriteMethod->SendTransactionViaKeystoreComplete(TransactionResponse);
}

void FLocalEVMThreadRunnable::Stop()
{
}
