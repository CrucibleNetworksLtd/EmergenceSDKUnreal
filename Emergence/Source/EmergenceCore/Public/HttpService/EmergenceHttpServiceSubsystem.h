// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "EmergenceHttpServiceSubsystem.generated.h"

/**
 * Handles closing of active requests when a PIE session ends, amongst other calls that need to be made by the HTTP service on 
 * game launch.
 */
UCLASS()
class EMERGENCECORE_API UEmergenceHttpServiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//holds a list of active requests for closing when the game ends
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> ActiveRequests;
};
