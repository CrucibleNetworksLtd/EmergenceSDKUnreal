// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "GetDataFromURL.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"

void UGetDataFromUrl::Activate()
{
	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetDataFromUrl>(this, &UGetDataFromUrl::GetDataFromUrl_HttpRequestComplete, this->Url);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetDataFromUrl request started (%s), calling GetDataFromUrl_HttpRequestComplete on request completed"), *this->Url);
}

void UGetDataFromUrl::GetDataFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UHttpHelperLibrary::GetResponseErrors(HttpRequest, HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UEmergenceErrorCode::Conv_ErrorCodeToInt(ResponseCode))) {
		OnGetDataFromUrlCompleted.Broadcast(TArray<uint8>(), FString(), ResponseCode, false);
		return;
	}
	
	OnGetDataFromUrlCompleted.Broadcast(HttpResponse->GetContent(), HttpResponse->GetContentAsString(), ResponseCode, true);
}
