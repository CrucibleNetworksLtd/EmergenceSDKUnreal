#include "EmergenceLocalEVMThread.h"


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
	UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), (char*)(Data->result));
	FString TransactionResponse = "";
	for (int i = 0; i < (int)Data->ResultLength * 2; i++) {
		TransactionResponse.AppendChar(((char*)(Data->result))[i]);
	}
	WriteMethod->SendTransactionViaKeystoreComplete(TransactionResponse);
}

void FLocalEVMThreadRunnable::Stop()
{
}
