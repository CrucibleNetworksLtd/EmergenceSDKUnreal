// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "AvatarByOwner.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceAvatarMetadata
{

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Creator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UriBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxTotalSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxTotalVertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GUID;

	FEmergenceAvatarMetadata() {};

	FEmergenceAvatarMetadata(FString _json_) {
		FEmergenceAvatarMetadata _tmpEmergenceAvatarMetadata;

		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceAvatarMetadata>(
			_json_,
			&_tmpEmergenceAvatarMetadata,
			0, 0);

		Name = _tmpEmergenceAvatarMetadata.Name;
		Creator = _tmpEmergenceAvatarMetadata.Creator;
		Type = _tmpEmergenceAvatarMetadata.Type;
		UriBase = _tmpEmergenceAvatarMetadata.UriBase;
		MaxTotalSize = _tmpEmergenceAvatarMetadata.MaxTotalSize;
		MaxTotalVertices = _tmpEmergenceAvatarMetadata.MaxTotalVertices;
		GUID = _tmpEmergenceAvatarMetadata.GUID;
	};

};

USTRUCT(BlueprintType)
struct FEmergenceAvatarResult
{

  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString avatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString contractAddress;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString tokenId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString tokenURI;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FEmergenceAvatarMetadata> Avatars;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 lastUpdated;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString chain;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FEmergenceInventoryItemMeta meta;

  FEmergenceAvatarResult() {};

  FEmergenceAvatarResult( FString _avatarId, FString _contractAddress, FString _tokenId, FString _tokenURI, int32 _lastUpdated, FString _chain, FEmergenceInventoryItemMeta _meta){

    avatarId = _avatarId;
    contractAddress = _contractAddress;
    tokenId = _tokenId;
	FJsonObjectConverter::JsonArrayStringToUStruct<FEmergenceAvatarMetadata>(_tokenURI, &this->Avatars, 0, 0);
    lastUpdated = _lastUpdated;
	chain = _chain;
	meta = _meta;
  }
  
  FEmergenceAvatarResult(FString _json_){
    FEmergenceAvatarResult _tmpEmergenceAvatarsResult;
    
	FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceAvatarResult>(_json_, &_tmpEmergenceAvatarsResult, 0, 0);
	
    avatarId = _tmpEmergenceAvatarsResult.avatarId;
    contractAddress = _tmpEmergenceAvatarsResult.contractAddress;
    tokenId = _tmpEmergenceAvatarsResult.tokenId;
    lastUpdated = _tmpEmergenceAvatarsResult.lastUpdated;
	chain = _tmpEmergenceAvatarsResult.chain;
	meta = _tmpEmergenceAvatarsResult.meta;

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(_json_);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		FJsonObjectConverter::JsonArrayToUStruct(JsonParsed->GetArrayField("tokenURI"), &Avatars, 0, 0);
	}
  }
  
};

UCLASS()
class EMERGENCE_API UAvatarByOwner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the all the avatars of a given address from the Avatar System.
	 * @param Address Address to get the avatars of.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Avatar Service")
	static UAvatarByOwner* AvatarByOwner(const UObject* WorldContextObject, const FString& Address);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByOwnerCompleted, const TArray<FEmergenceAvatarResult>&, Avatars, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByOwnerCompleted OnAvatarByOwnerCompleted;
private:
	void AvatarByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Address;
	const UObject* WorldContextObject;

	TArray<TPair<FHttpRequestRef, FEmergenceAvatarResult>> Requests;
	TArray<FEmergenceAvatarResult> Results;
};