// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"


#if PLATFORM_WINDOWS
#include "Windows/WindowsSystemIncludes.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif
void ULocalEmergenceServer::LaunchLocalServerProcess(bool LaunchHidden)
{
	if (!UHttpHelperLibrary::APIBase.IsEmpty()) { //if we think we may already have a server
		UE_LOG(LogEmergenceHttp, Error, TEXT("The server has already been started or was closed without calling KillLocalServerProcess. This causes Emergence's internal state to get messed up, don't do this."));
		return;
	}
#if PLATFORM_WINDOWS
	FString EmergenceServerBinariesPath = FString(FPlatformProcess::BaseDir()) + "/EmergenceEVMLocalServer.exe";
	FString EmergenceServerPluginPath = FString(FPaths::ProjectPluginsDir() + "Emergence/EmergenceServer/Windows/EmergenceEVMLocalServer.exe");
#else
	FString EmergenceServerBinariesPath = FString(FPlatformProcess::BaseDir()) + "/EmergenceEVMLocalServer";
	FString EmergenceServerPluginPath = FString(FPaths::ProjectPluginsDir() + "Emergence/EmergenceServer/Mac/EmergenceEVMLocalServer");
#endif	
	FString LoadPath;
	if (FPaths::FileExists(EmergenceServerBinariesPath)) {
		LoadPath = EmergenceServerBinariesPath;
	}
	else if (FPaths::FileExists(EmergenceServerPluginPath)) {
		LoadPath = EmergenceServerPluginPath;
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("Couldn't find EmergenceServer in binaries or plugin path locations. Make sure you have the server files copied to Plugins/Emergence/EmergenceServer/EmergenceEVMLocalServer.exe"));
		return;
	}

	if (GConfig) {
		FString EmergenceCustomServerLocation;
		if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("CustomEmergenceServerLocation"), EmergenceCustomServerLocation, GGameIni)) {
			FParse::Value(*EmergenceCustomServerLocation, TEXT("FilePath="), EmergenceCustomServerLocation);
			if (FPaths::FileExists(*EmergenceCustomServerLocation)) {
				LoadPath = EmergenceCustomServerLocation;
				UE_LOG(LogEmergenceHttp, Warning, TEXT("Found EmergenceServer at override path (%s)."), *EmergenceCustomServerLocation);
			}
		}
	}

	UE_LOG(LogEmergenceHttp, Display, TEXT("Loading Emergence Server from path: %s"), *LoadPath);

	const FString JsonArgs("\"{\\\"Name\\\":\\\"Emergence\\\",\\\"Description\\\":\\\"Unreal Engine Wallet Connect\\\",\\\"Icons\\\":\\\"https:\\/\\/crucible.network\\/wp-content\\/uploads\\/2020\\/10\\/cropped-crucible_favicon-32x32.png\\\",\\\"URL\\\":\\\"https:\\/\\/crucible.network\\\"}\"");


	FString ServerURL = "http://localhost:" + FString::FromInt(GetNextFreePort());

	UHttpHelperLibrary::APIBase = ServerURL + "/api/";

	//Add the args
	TArray<FString> Args = {
		"--urls=\"" + ServerURL + "\"",
		"--walletconnect=" + JsonArgs,
		"--processid=" + FString::FromInt(FPlatformProcess::GetCurrentProcessId())
	};

	//combine the args
	FString ArgString = "";
	for (int i = 0; i < Args.Num(); i++) {
		if (i != 0) { //add a space before the next arg
			ArgString = ArgString + " ";
		}
		UE_LOG(LogEmergenceHttp, Display, TEXT("calling argument [%d]: %s"), i, *Args[i]);
		ArgString = ArgString + Args[i];
	}
	UE_LOG(LogEmergenceHttp, Display, TEXT("Total argument lenth is %d"), ArgString.Len());
	//create the process
	FPlatformProcess::CreateProc(*LoadPath, *ArgString, false, true, LaunchHidden, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogEmergenceHttp, Display, TEXT("calling: %s %s"), *LoadPath, *ArgString);
}

void ULocalEmergenceServer::KillLocalServerProcess()
{
	UHttpHelperLibrary::ExecuteHttpRequest<ULocalEmergenceServer>(nullptr, nullptr, UHttpHelperLibrary::APIBase + "finish");
	UE_LOG(LogEmergenceHttp, Display, TEXT("KillLocalServerProcess request started..."));
	UHttpHelperLibrary::APIBase.Empty();
}

#if PLATFORM_WINDOWS
bool ULocalEmergenceServer::GetUsedTCPPorts(TArray<int>& UsedPorts) {
	PMIB_TCPTABLE2 pTcpTable;
	ULONG ulSize = 0;
	DWORD dwRetVal = 0;

	int i;

	pTcpTable = (MIB_TCPTABLE2*)MALLOC(sizeof(MIB_TCPTABLE2));
	if (pTcpTable == NULL) {
		//Error allocating memory
		return false;
	}

	ulSize = sizeof(MIB_TCPTABLE);
	// Make an initial call to GetTcpTable2 to get the necessary size into the ulSize variable
	if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, true)) ==
		ERROR_INSUFFICIENT_BUFFER) {
		FREE(pTcpTable);
		pTcpTable = (MIB_TCPTABLE2*)MALLOC(ulSize);
		if (pTcpTable == NULL) {
			//Error allocating memory
			return false;
		}
	}
	// Make a second call to GetTcpTable2 to get the actual data we require
	if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, true)) == NO_ERROR) {
		for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
			UsedPorts.AddUnique(ntohs((u_short)pTcpTable->table[i].dwLocalPort));
		}
	}
	else {
		//failed
		FREE(pTcpTable);
		return false;
	}

	if (pTcpTable != NULL) {
		FREE(pTcpTable);
		pTcpTable = NULL;
	}

	return true;
}

int ULocalEmergenceServer::GetNextFreePort()
{
	int LookingPort = 57000;

	TArray<int> UsedPorts;
	ULocalEmergenceServer::GetUsedTCPPorts(UsedPorts);

	while (LookingPort < 65535 && UsedPorts.Contains(LookingPort)) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("Port %d is taken, checking next port..."), LookingPort);
		LookingPort++;
	}

	if (LookingPort < 65535) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("Found free port %d"), LookingPort);
		return LookingPort;
	}

	UE_LOG(LogEmergenceHttp, Error, TEXT("Couldn't find a free port!"));
	return -1;
}
#else
bool ULocalEmergenceServer::GetUsedTCPPorts(TArray<int>& UsedPorts) {
	return false;
}

int ULocalEmergenceServer::GetNextFreePort()
{
	return 57000;
}
#endif
