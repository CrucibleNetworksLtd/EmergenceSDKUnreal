// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Emergence.h"
#include "GetTextureFromURL.generated.h"
/**
 */
UCLASS()
class EMERGENCE_API UGetTextureFromUrl : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	//Takes a URL string and gets the PNG, JPEG or GIF from it. Will not work with anything other than PNGs, JPEGs or GIFs.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Get Texture From URL (PNG, JPEG or GIF)", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Interface Helpers")
	static UGetTextureFromUrl* TextureFromUrl(const FString& Url, UObject* WorldContextObject, bool AllowCacheUsage = true) {
		UGetTextureFromUrl* BlueprintNode = NewObject<UGetTextureFromUrl>(); //I don't know why, but every time I tried to put this in the cpp file it wouldn't link it properly and would fail to compile. If you think you can fix it, go ahead.
		
		BlueprintNode->Url = FString(Url);
		
		BlueprintNode->AllowCacheUsage = AllowCacheUsage;
		BlueprintNode->WorldContextObject = WorldContextObject;
		BlueprintNode->RegisterWithGameInstance(WorldContextObject);
		return BlueprintNode;
	}

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTextureFromUrlCompleted, UTexture2D*, Texture, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTextureFromUrlCompleted OnGetTextureFromUrlCompleted;

private:
	void GetTextureFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void ConvertGIFtoPNG_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void WaitOneFrame();

	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);
	
	FString Url;
	bool AllowCacheUsage;
	UObject* WorldContextObject;
	FTimerHandle Timer;
	UTexture2D** CachedTexturePtr;
};
