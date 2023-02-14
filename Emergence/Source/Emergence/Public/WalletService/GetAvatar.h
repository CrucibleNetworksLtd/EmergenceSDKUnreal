// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetAvatar.generated.h"

UCLASS()
class EMERGENCE_API UGetAvatar : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Used for getting ERC721 images, such as avatars.
	 * @param ImageMetadataURI Usually the output of a ReadMethod call.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ERC721 Image from URI", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Interface Helpers")
	static UGetAvatar* GetAvatar(UObject* WorldContextObject, FString ImageMetadataURI);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetAvatarCompleted, UTexture2D*, Avatar, FString, Name, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetAvatarCompleted OnGetAvatarCompleted;
private:
	void GetAvatar_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void AvatarReturned(UTexture2D* Texture, EErrorCode StatusCode);
	UObject* WorldContextObject;
	FString ImageMetadataURI;
	FString ERC721Name;
	int TokenId;
};
