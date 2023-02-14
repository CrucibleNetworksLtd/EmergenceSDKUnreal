// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetPersonaByID.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UGetPersonaByID : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static UGetPersonaByID* GetPersonaByID(UObject* WorldContextObject, const FString& personaID);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetPersonaByIDCompleted, FEmergencePersona, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetPersonaByIDCompleted OnGetPersonaByIDCompleted;
private:
	void GetPersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString PersonaID;
	UObject* WorldContextObject;
};
