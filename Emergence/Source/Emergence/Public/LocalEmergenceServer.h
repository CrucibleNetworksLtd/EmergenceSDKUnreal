// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Emergence.h"
#include "LocalEmergenceServer.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API ULocalEmergenceServer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//Launches the local server process. If LaunchHidden is true, it will not be visible to the user (except in Task Manager).
	UFUNCTION(BlueprintCallable, Category = "Emergence|Local Server Control")
	static void LaunchLocalServerProcess(bool LaunchHidden = true);

	//Kills the local server process.
	UFUNCTION(BlueprintCallable, Category = "Emergence|Local Server Control")
	static void KillLocalServerProcess();

	//Gets the used TCP ports. Returns false if something goes wrong.
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	static bool GetUsedTCPPorts(TArray<int>& UsedPorts);

private:
	static int GetNextFreePort();
};
