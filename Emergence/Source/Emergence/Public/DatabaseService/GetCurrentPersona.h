// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetCurrentPersona.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UGetCurrentPersona : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static UGetCurrentPersona* GetCurrentPersona(UObject* WorldContextObject);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetCurrentPersonaCompleted, FEmergencePersona, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetCurrentPersonaCompleted OnGetCurrentPersonaCompleted;
private:
	void GetCurrentPersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
};
