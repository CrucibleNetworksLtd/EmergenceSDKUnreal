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

	UHttpHelperLibrary::ExecuteHttpRequest<UGetTextureFromUrl>(this, &UGetTextureFromUrl::GetTextureFromUrl_HttpRequestComplete, this->Url);
	if (AllowCacheUsage) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), didn't find it in the cache, calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
	}
	else {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
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
			UE_LOG(LogEmergenceHttp, Display, TEXT("Found a GIF after GetTextureFromURL, sending for conversion..."));
			TArray<TPair<FString, FString>> Headers;
			Headers.Add(TPair<FString, FString>("Content-Type", "multipart/form-data; boundary=EmergenceBoundary"));
			Headers.Add(TPair<FString, FString>("accept", "*/*"));
			FHttpRequestRef Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetTextureFromUrl>(this, &UGetTextureFromUrl::ConvertGIFtoPNG_HttpRequestComplete, UHttpHelperLibrary::APIBase + "gifTojpeg", "POST", 60.0F, Headers, "", false);
			
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

			Request->SetContent(data);
			Request->ProcessRequest();
			return; //don't continue, we'll handle all the conversions once ConvertGIFtoPNG_HttpRequestComplete returns
		}
	}

	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetTextureFromUrlCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Add(this->Url, QRCodeTexture);
		return;
	}
	else {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientWrongType);
	}
}

void UGetTextureFromUrl::ConvertGIFtoPNG_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!bSucceeded) {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, UErrorCodeFunctionLibrary::Conv_IntToErrorCode(HttpResponse->GetResponseCode()));
		return;
	}
	UE_LOG(LogEmergenceHttp, Display, TEXT("Convert GIF to PNG returned, turning it into a texture..."));
	TArray<uint8> ResponceBytes = HttpResponse->GetContent();
	UE_LOG(LogEmergenceHttp, Display, TEXT("Content as string: %s"), *HttpResponse->GetContentAsString());
	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetTextureFromUrlCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Add(this->Url, QRCodeTexture);
		return;
	}
	else {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientFailed); //if this happens, we have no idea whats going on
	}
}

bool UGetTextureFromUrl::RawDataToBrush(FName ResourceName, const TArray< uint8 >& InRawData, UTexture2D*& LoadedT2D)
{
	int32 Width;
	int32 Height;

	TArray<uint8> DecodedImage;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper;

	if (InRawData.Num() == 0) { //if there is no raw data, fail out
		return false;
	}

	if (InRawData[0] == 0x89) {
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InRawData[0] == 0xFF && InRawData[1] == 0xD8 && InRawData[2] == 0xFF) {
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else {
		return false;
	}

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InRawData.GetData(), InRawData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!LoadedT2D) return false;

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			LoadedT2D->UpdateResource();
			return true;
		}
	}
	return false;
}

void UGetTextureFromUrl::WaitOneFrame()
{
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(Timer);
	OnGetTextureFromUrlCompleted.Broadcast(*CachedTexturePtr, EErrorCode::EmergenceOk);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), found it in cache, returning"), *this->Url);
	Timer.Invalidate();
}
