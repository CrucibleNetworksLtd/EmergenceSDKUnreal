// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Emergence.h"
#include "GetDataFromUrl.generated.h"
/**
 */
UCLASS()
class EMERGENCE_API UGetDataFromUrl : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	//Takes a URL string and gets the data.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Get Data From URL", WorldContext = "WorldContextObject"), Category = "Emergence|Helpers")
	static UGetDataFromUrl* GetDataFromUrl(const UObject* WorldContextObject, const FString& Url) {
		UGetDataFromUrl* BlueprintNode = NewObject<UGetDataFromUrl>(); //I don't know why, but every time I tried to put this in the cpp file it wouldn't link it properly and would fail to compile. If you think you can fix it, go ahead.
		
		BlueprintNode->Url = FString(Url);
		BlueprintNode->WorldContextObject = WorldContextObject;
		return BlueprintNode;
	}

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetDataFromUrlCompleted, const TArray<uint8>&, Data, EErrorCode, StatusCode, bool, Success);

	UPROPERTY(BlueprintAssignable)
	FOnGetDataFromUrlCompleted OnGetDataFromUrlCompleted;

private:
	void GetDataFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString Url;
	const UObject* WorldContextObject;
};
