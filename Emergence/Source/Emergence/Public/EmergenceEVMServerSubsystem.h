// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpRouteHandle.h"
#include "EmergenceEVMServerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UEmergenceEVMServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//holds a list of active requests for closing when the game ends
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> ActiveRequests;
};
