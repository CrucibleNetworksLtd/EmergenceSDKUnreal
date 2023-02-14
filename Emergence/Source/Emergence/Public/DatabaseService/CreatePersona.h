// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "CreatePersona.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCreatePersona : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static UCreatePersona* CreatePersona(UObject* WorldContextObject, FEmergencePersona Persona);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreatePersonaCompleted, FEmergencePersona, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCreatePersonaCompleted OnCreatePersonaCompleted;
private:
	void CreatePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	FEmergencePersona TempPersona;
};
