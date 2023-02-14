// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "SetActivePersona.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API USetActivePersona : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static USetActivePersona* SetActivePersona(UObject* WorldContextObject, const FString& personaID);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetActivePersonaCompleted, FEmergencePersona, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnSetActivePersonaCompleted OnSetActivePersonaCompleted;
private:
	void SetActivePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	FString PersonaID;
};
