// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Emergence.h"
DEFINE_LOG_CATEGORY(LogEmergence)
DEFINE_LOG_CATEGORY(LogEmergenceHttp)
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceChainObject.h"
#include "EmergenceContract.h"
#include "EmergenceLocalEVMThread.h"

#define LOCTEXT_NAMESPACE "FEmergenceModule"

void FEmergenceModule::StartupModule()
{

}

void FEmergenceModule::ShutdownModule()
{

}

void FEmergenceModule::SendTransactionViaKeystore(UWriteMethod* WriteMethod, UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse)
{
#if PLATFORM_WINDOWS
	FString BaseDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();
	// Add on the relative location of the third party dll and load it
	FString LibraryPath;

	FString DllDirectory = BaseDir + "/EmergenceDll/Win64/";
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	LibraryPath = FPaths::ConvertRelativePathToFull(DllDirectory + "nativehost.dll");

	if (LibraryPath.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("Failed to load library, library path empty."));
		WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
		return; //error out
	}
	
	if (!ExampleLibraryHandle) { //if we don't have a handle on the lib already
		ExampleLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);
	}

	if (ExampleLibraryHandle && !ExampleLibraryFunction) { //if we now have a handle (not a guarantee), and we don't have a library
		ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, TEXT("entry"));
	}
	
	if(!ExampleLibraryHandle || !ExampleLibraryFunction) { //if we don't have either of them by now
		UE_LOG(LogEmergence, Error, TEXT("Failed to load library."));
		WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
		return; //give up
	}
	

	FString GasPriceInternal;
	GasPriceInternal.Empty(0);
	if (!GasPrice.IsEmpty()) {
		GasPriceInternal = GasPrice;
	}

	char* ABI = new char[Deployment->Contract->ABI.GetCharArray().Num()];
	TCHAR* CharArray = Deployment->Contract->ABI.GetCharArray().GetData();
	for (int i = 0; i < Deployment->Contract->ABI.GetCharArray().Num(); i++) {
		TCHAR c = CharArray[i];
		ABI[i] = (ANSICHAR)c;
	}

	EmergenceLocalEVMJSON* jsonArgs = new EmergenceLocalEVMJSON{
		0, //unused
		"", //unused
		TCHAR_TO_ANSI(Deployment->Address.GetCharArray().GetData()),
		ABI,
		TCHAR_TO_ANSI(Deployment->Blockchain->Name.ToString().GetCharArray().GetData()),
		TCHAR_TO_ANSI(Deployment->Blockchain->NodeURL.GetCharArray().GetData()),
		TCHAR_TO_ANSI(MethodName.GetCharArray().GetData()),
		"", //no longer used
		"", //no longer used
		R"()", //no longer used
		TCHAR_TO_ANSI(FString::FromInt(Deployment->Blockchain->ChainID).GetCharArray().GetData()),
		TCHAR_TO_ANSI(GasPriceInternal.GetCharArray().GetData()),
		TCHAR_TO_ANSI(Value.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PrivateKey.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PublicKey.GetCharArray().GetData()),
		nullptr, //return address
		0 //return length
	};

	if (ExampleLibraryHandle)
	{
		if (ExampleLibraryFunction) {

			FLocalEVMThreadRunnable* Runnable = new FLocalEVMThreadRunnable();
			Runnable->Data = new EmergenceLocalEVMJSON(*jsonArgs);
			//wchar_t* NewFullpath = static_cast<wchar_t*>((TCHAR*)*LibraryPath);
			const wchar_t* MyWideCharString = (*LibraryPath);
			wcscpy(Runnable->fullpath, MyWideCharString);
			Runnable->length = LibraryPath.Len();
			Runnable->ExampleLibraryFunction = ExampleLibraryFunction;
			Runnable->WriteMethod = WriteMethod;
			auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVMThread"));
			return;
		}
	}

	//if it hasn't returned by now something has gone really wrong
	UE_LOG(LogEmergence, Error, TEXT("Failed to load library, handles weren't valid."));
	WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
	return;

#else //any other platform
	UE_LOG(LogEmergence, Error, TEXT("Sending Transactions via a private key is only available on Windows."));
	WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
	return;
#endif	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceModule, Emergence)