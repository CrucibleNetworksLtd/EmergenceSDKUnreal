// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetPersonas.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UGetPersonas : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static UGetPersonas* GetPersonas(UObject* WorldContextObject);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetPersonasCompleted, FEmergencePersonaListResponse, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetPersonasCompleted OnGetPersonasCompleted;
private:
	void GetPersonas_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
};
