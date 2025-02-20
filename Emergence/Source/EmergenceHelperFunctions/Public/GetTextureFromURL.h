// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceCancelableAsyncBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION > 4
#include "Engine/TimerHandle.h"
#endif
#include "GetTextureFromURL.generated.h"
/**
 */
UCLASS()
class EMERGENCEHELPERFUNCTIONS_API UGetTextureFromUrl : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	//Takes a URL string and gets the PNG, JPEG or GIF from it. Will not work with anything other than PNGs, JPEGs or GIFs.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Get Texture From URL (PNG, JPEG or GIF)", WorldContext = "WorldContextObject"), Category = "Emergence|Helpers")
	static UGetTextureFromUrl* TextureFromUrl(const FString& Url, UObject* WorldContextObject, bool AllowCacheUsage = true) {
		UGetTextureFromUrl* BlueprintNode = NewObject<UGetTextureFromUrl>(); //I don't know why, but every time I tried to put this in the cpp file it wouldn't link it properly and would fail to compile. If you think you can fix it, go ahead.
		
		BlueprintNode->Url = FString(Url);
		
		BlueprintNode->AllowCacheUsage = AllowCacheUsage;
		BlueprintNode->WorldContextObject = WorldContextObject;
		BlueprintNode->RegisterWithGameInstance(WorldContextObject);
		return BlueprintNode;
	}

	virtual void Cancel();

	virtual bool IsActive() const;

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTextureFromUrlCompleted, UTexture2D*, Texture, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTextureFromUrlCompleted OnGetTextureFromUrlCompleted;

	FHttpRequestPtr GetDataRequest;
	FHttpRequestPtr ConvertGifRequest;

private:
	void GetTextureFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void ConvertGIFtoPNG_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void WaitOneFrame();
	
	static TMap<FString, UTexture2D*> DownloadedImageCache;
	
	FString Url;
	bool AllowCacheUsage;
	
	FTimerHandle Timer;
	UTexture2D** CachedTexturePtr;
};
