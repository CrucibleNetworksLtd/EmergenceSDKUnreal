// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "GetTextureFromURL.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"
#include "EmergenceSingleton.h"
#include "TimerManager.h"

void UGetTextureFromUrl::Activate()
{
	if (AllowCacheUsage) {
		CachedTexturePtr = UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Find(this->Url);
		if (CachedTexturePtr) {
			//even though we have this ready, we need to send it with a tiny timer or the node won't make sense (it will return its completed before its regular node flow has completed, THAT WOULD BE REALLY REALLY BAD AND CONFUSING).
			this->Timer = WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGetTextureFromUrl::WaitOneFrame);
			return;
		}
	}

	GetDataRequest = UHttpHelperLibrary::ExecuteHttpRequest<UGetTextureFromUrl>(this, &UGetTextureFromUrl::GetTextureFromUrl_HttpRequestComplete, this->Url);
	if (AllowCacheUsage) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), didn't find it in the cache, calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
	}
	else {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), not using cache, calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
	}
}

void UGetTextureFromUrl::GetTextureFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(ResponseCode))) {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, ResponseCode);
		return;
	}

	TArray<uint8> ResponceBytes = HttpResponse->GetContent();

	//Checking for GIFs
	if (ResponceBytes.Num() > 0) { //if its not empty
		if (ResponceBytes[0] == 0x47 && ResponceBytes[1] == 0x49 && ResponceBytes[2] == 0x46) { //this is a GIF
#if PLATFORM_WINDOWS
			UE_LOG(LogEmergenceHttp, Display, TEXT("Found a GIF after GetTextureFromURL, sending for conversion..."));
			TArray<TPair<FString, FString>> Headers;
			Headers.Add(TPair<FString, FString>("Content-Type", "multipart/form-data; boundary=EmergenceBoundary"));
			Headers.Add(TPair<FString, FString>("accept", "*/*"));
			ConvertGifRequest = UHttpHelperLibrary::ExecuteHttpRequest<UGetTextureFromUrl>(this, &UGetTextureFromUrl::ConvertGIFtoPNG_HttpRequestComplete, UHttpHelperLibrary::APIBase + "gifTojpeg", "POST", 60.0F, Headers, "", false);

			FString a = "\r\n--EmergenceBoundary\r\n";
			FString b = "Content-Disposition: form-data; name=\"file\";  filename=\"gif.gif\"\r\n";

			FString c = "Content-Type: image/gif\r\n\r\n";
			//d = UpFileRawData
			FString e = "\r\n--EmergenceBoundary--\r\n";


			TArray<uint8> data;
			data.Append((uint8*)TCHAR_TO_UTF8(*a), a.Len());
			data.Append((uint8*)TCHAR_TO_UTF8(*b), b.Len());
			data.Append((uint8*)TCHAR_TO_UTF8(*c), c.Len());
			data.Append(ResponceBytes);
			data.Append((uint8*)TCHAR_TO_UTF8(*e), e.Len());

			ConvertGifRequest->SetContent(data);
			ConvertGifRequest->ProcessRequest();
			return; //don't continue, we'll handle all the conversions once ConvertGIFtoPNG_HttpRequestComplete returns
#else
			return;
#endif
		}
	}

	UTexture2D* QRCodeTexture;
	if (UEmergenceSingleton::RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetTextureFromUrlCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		//if we still have a world context object
		if (WorldContextObject && WorldContextObject->IsValidLowLevel()) {
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Add(this->Url, QRCodeTexture);
		}
		return;
	}
	else {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientWrongType);
	}
}

void UGetTextureFromUrl::ConvertGIFtoPNG_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!bSucceeded || (ResponseCode != EErrorCode::Ok)) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("Failed to get converted GIF, reason number: %d"), (int)ResponseCode);
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, UErrorCodeFunctionLibrary::Conv_IntToErrorCode(HttpResponse->GetResponseCode()));
		return;
	}
	UE_LOG(LogEmergenceHttp, Display, TEXT("Convert GIF to PNG returned, turning it into a texture..."));
	TArray<uint8> ResponceBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (UEmergenceSingleton::RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetTextureFromUrlCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Add(this->Url, QRCodeTexture);
		return;
	}
	else {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientFailed); //if this happens, we have no idea whats going on
	}
}

void UGetTextureFromUrl::WaitOneFrame()
{
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(Timer);
	OnGetTextureFromUrlCompleted.Broadcast(*CachedTexturePtr, EErrorCode::EmergenceOk);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), found it in cache, returning"), *this->Url);
	Timer.Invalidate();
}

bool UGetTextureFromUrl::IsActive() const
{
	return GetDataRequest->GetStatus() == EHttpRequestStatus::Processing || ConvertGifRequest->GetStatus() == EHttpRequestStatus::Processing;
}

void UGetTextureFromUrl::Cancel() {
	if (GetDataRequest) {
		GetDataRequest->OnProcessRequestComplete().Unbind();
		GetDataRequest->CancelRequest();
	}
	if (ConvertGifRequest) {
		ConvertGifRequest->OnProcessRequestComplete().Unbind();
		ConvertGifRequest->CancelRequest();
	}
}