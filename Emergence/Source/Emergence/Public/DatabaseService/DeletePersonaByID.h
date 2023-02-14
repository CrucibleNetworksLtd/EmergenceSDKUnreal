// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "DeletePersonaByID.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UDeletePersonaByID : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Persona Methods")
	static UDeletePersonaByID* DeletePersonaByID(UObject* WorldContextObject, const FString& PersonaID);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeletePersonaByIDCompleted, FEmergencePersona, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnDeletePersonaByIDCompleted OnDeletePersonaByIDCompleted;
private:
	void DeletePersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	FString PersonaID;
};
