// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "EmergenceCore.h"
#include "GetDataFromURL.generated.h"
/**
 */
UCLASS()
class EMERGENCEHELPERFUNCTIONS_API UGetDataFromUrl : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Takes a URL string and gets the data. Can be HTTP or IPFS.
	 * @param Url The Url to get data from. Can be HTTP or IPFS.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Get Data From URL", WorldContext = "WorldContextObject"), Category = "Emergence|Helpers")
	static UGetDataFromUrl* GetDataFromUrl(UObject* WorldContextObject, const FString& Url) {
		UGetDataFromUrl* BlueprintNode = NewObject<UGetDataFromUrl>(); //I don't know why, but every time I tried to put this in the cpp file it wouldn't link it properly and would fail to compile. If you think you can fix it, go ahead.
		
		BlueprintNode->Url = FString(Url);
		BlueprintNode->WorldContextObject = WorldContextObject;
		BlueprintNode->RegisterWithGameInstance(WorldContextObject);
		return BlueprintNode;
	}

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGetDataFromUrlCompleted, const TArray<uint8>&, Data, FString, String, EErrorCode, StatusCode, bool, Success);

	UPROPERTY(BlueprintAssignable)
	FOnGetDataFromUrlCompleted OnGetDataFromUrlCompleted;
	
private:
	void GetDataFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString Url;
	
};
