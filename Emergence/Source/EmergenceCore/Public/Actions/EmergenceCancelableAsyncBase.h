// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncActionBase.h"
#include "EmergenceCancelableAsyncBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, meta = (ExposedAsyncProxy = Requests))
class EMERGENCECORE_API UEmergenceCancelableAsyncBase : public UEmergenceAsyncActionBase
{
	GENERATED_BODY()
	
	//Cancels all requests (web requests, etc) related to this asynchronous node.
	UFUNCTION(BlueprintCallable, Category = "Requests")
	virtual void Cancel();

	//Returns true if requests (web requests, etc) related to this asynchronous node are still "in flight" and this hasn't been cancelled.
	UFUNCTION(BlueprintCallable, Category = "Requests", meta = (KeyWords = "status, stop"))
	virtual bool IsActive() const;

	//This should be checked prior to broadcasting delegates to check this node hasn't been cancelled.
	virtual bool ShouldBroadcastDelegates() const;
};
