// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "EmergenceInteroperableAssetElement.h"
#include "NFTElement.generated.h"


USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FNFTElementInnerAssetMediaType
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System|MediaType", EditAnywhere, BlueprintReadWrite)
    FString Type;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|MediaType", EditAnywhere, BlueprintReadWrite)
    FString Element;

    FNFTElementInnerAssetMediaType() {};

    FNFTElementInnerAssetMediaType(FString _Type, FString _Element) {

        Type = _Type;
        Element = _Element;

    }

};

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FNFTElementInnerAsset
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Asset", EditAnywhere, BlueprintReadWrite)
    FNFTElementInnerAssetMediaType MediaType;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Asset", EditAnywhere, BlueprintReadWrite)
    FString AssetLocation;

    FNFTElementInnerAsset() {};

    FNFTElementInnerAsset(FNFTElementInnerAssetMediaType _MediaType, FString _AssetLocation) {

        MediaType = _MediaType;
        AssetLocation = _AssetLocation;

    }

};

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FNFTElementInnerChain
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Chain", EditAnywhere, BlueprintReadWrite)
    bool IsTestnet;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Chain", EditAnywhere, BlueprintReadWrite)
    FString ChainName;

    FNFTElementInnerChain() {};

    FNFTElementInnerChain(bool _IsTestnet, FString _ChainName) {

        IsTestnet = _IsTestnet;
        ChainName = _ChainName;

    }

};

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FNFTElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString Address;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString NFTName;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Attributes;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FNFTElementInnerChain Chain;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString TokenNumber;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    int32 TokenType;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString CollectionName;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    int32 PrimaryAsset;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString Creator;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString Owner;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    TArray<FNFTElementInnerAsset> Assets;

    FNFTElementInner() {};

    FNFTElementInner(FString _Address, FString _NFTName, FString _Description, TMap<FString,FString> _Attributes, FNFTElementInnerChain _Chain, FString _TokenNumber, int32 _TokenType, FString _CollectionName, int32 _PrimaryAsset, FString _ElementName, FString _Creator, FString _Owner, TArray<FNFTElementInnerAsset> _Assets) {

        Address = _Address;
        NFTName = _NFTName;
        Description = _Description;
        Attributes = _Attributes;
        Chain = _Chain;
        TokenNumber = _TokenNumber;
        TokenType = _TokenType;
        CollectionName = _CollectionName;
        PrimaryAsset = _PrimaryAsset;
        ElementName = _ElementName;
        Creator = _Creator;
        Owner = _Owner;
        Assets = _Assets;

    }

    FNFTElementInner(FString _json_) {
        FNFTElementInner _tmpEmergenceInteroperableAssetNFTElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FNFTElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetNFTElementInner,
            0, 0);

        Address = _tmpEmergenceInteroperableAssetNFTElementInner.Address;
        NFTName = _tmpEmergenceInteroperableAssetNFTElementInner.NFTName;
        Description = _tmpEmergenceInteroperableAssetNFTElementInner.Description;
        //Attributes are delt with below
        Chain = _tmpEmergenceInteroperableAssetNFTElementInner.Chain;
        TokenNumber = _tmpEmergenceInteroperableAssetNFTElementInner.TokenNumber;
        TokenType = _tmpEmergenceInteroperableAssetNFTElementInner.TokenType;
        CollectionName = _tmpEmergenceInteroperableAssetNFTElementInner.CollectionName;
        PrimaryAsset = _tmpEmergenceInteroperableAssetNFTElementInner.PrimaryAsset;
        ElementName = _tmpEmergenceInteroperableAssetNFTElementInner.ElementName;
        Creator = _tmpEmergenceInteroperableAssetNFTElementInner.Creator;
        Owner = _tmpEmergenceInteroperableAssetNFTElementInner.Owner;
        Assets = _tmpEmergenceInteroperableAssetNFTElementInner.Assets;
		
		TSharedPtr<FJsonValue> JsonValue;
		TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*_json_);
		if (FJsonSerializer::Deserialize(JsonReader, JsonValue) && JsonValue.IsValid())
		{
            for (auto AttributeKeyValue : JsonValue->AsObject()->GetObjectField("Attributes")->Values) { //for some reason attributes is a object, not an array
                FString Key = AttributeKeyValue.Key;
                FString Value = AttributeKeyValue.Value->AsString();
                Attributes.Add(Key, Value);
            }
		}
    }

};

UCLASS(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
class EMERGENCEIAS_API UNFTElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UNFTElement() {
        ElementName = "NFT";
    }

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|Element", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FNFTElementInner NFTElementData;
    
};