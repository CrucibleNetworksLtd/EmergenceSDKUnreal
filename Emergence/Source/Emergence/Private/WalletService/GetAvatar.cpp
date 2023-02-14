// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetAvatar.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "GetTextureFromURL.h"

UGetAvatar* UGetAvatar::GetAvatar(UObject* WorldContextObject, FString ImageMetadataURI)
{
	UGetAvatar* BlueprintNode = NewObject<UGetAvatar>();
	BlueprintNode->ImageMetadataURI = ImageMetadataURI;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetAvatar::Activate()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UGetAvatar>(
		this, 
		&UGetAvatar::GetAvatar_HttpRequestComplete, 
		ImageMetadataURI);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetAvatar request started with JSON, calling GetAvatar_HttpRequestComplete on request completed."));
}

void UGetAvatar::GetAvatar_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		ERC721Name = JsonObject.GetStringField("name");
		UGetTextureFromUrl* GetTextureFromUrlRequest = UGetTextureFromUrl::TextureFromUrl(JsonObject.GetStringField("image"), this->WorldContextObject, true);
		GetTextureFromUrlRequest->OnGetTextureFromUrlCompleted.AddDynamic(this, &UGetAvatar::AvatarReturned);
		GetTextureFromUrlRequest->Activate();
	}
	else {
		OnGetAvatarCompleted.Broadcast(nullptr, FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetAvatar", StatusCode);
	}
	SetReadyToDestroy();
}

void UGetAvatar::AvatarReturned(UTexture2D* Texture, EErrorCode StatusCode)
{
	OnGetAvatarCompleted.Broadcast(Texture, this->ERC721Name, StatusCode);
}
