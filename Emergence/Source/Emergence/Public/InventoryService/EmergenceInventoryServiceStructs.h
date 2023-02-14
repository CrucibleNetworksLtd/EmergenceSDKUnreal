// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EmergenceInventoryServiceStructs.generated.h"


USTRUCT(BlueprintType)
struct FEmergenceInventoryItemsMetaContent
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    FString url;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    FString representation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    FString mimeType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    int32 size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    int32 width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Content")
    int32 height;

    FEmergenceInventoryItemsMetaContent() {};

    FEmergenceInventoryItemsMetaContent(FString _url, FString _representation, FString _mimeType, int32 _size, int32 _width, int32 _height) {

        url = _url;
        representation = _representation;
        mimeType = _mimeType;
        size = _size;
        width = _width;
        height = _height;

    }

    FEmergenceInventoryItemsMetaContent(FString _json_) {
        FEmergenceInventoryItemsMetaContent _tmpEmergenceInventoryItemsMetaContent;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventoryItemsMetaContent>(
            _json_,
            &_tmpEmergenceInventoryItemsMetaContent,
            0, 0);

        url = _tmpEmergenceInventoryItemsMetaContent.url;
        representation = _tmpEmergenceInventoryItemsMetaContent.representation;
        mimeType = _tmpEmergenceInventoryItemsMetaContent.mimeType;
        size = _tmpEmergenceInventoryItemsMetaContent.size;
        width = _tmpEmergenceInventoryItemsMetaContent.width;
        height = _tmpEmergenceInventoryItemsMetaContent.height;
    }

};

USTRUCT(BlueprintType)
struct FEmergenceInventoryItemsMetaAttribute
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Attribute")
    FString key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta|Attribute")
    FString value;

    FEmergenceInventoryItemsMetaAttribute() {};

    FEmergenceInventoryItemsMetaAttribute(FString _key, FString _value) {

        key = _key;
        value = _value;

    }

    FEmergenceInventoryItemsMetaAttribute(FString _json_) {
        FEmergenceInventoryItemsMetaAttribute _tmpEmergenceInventoryItemsMetaAttribute;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventoryItemsMetaAttribute>(
            _json_,
            &_tmpEmergenceInventoryItemsMetaAttribute,
            0, 0);

        key = _tmpEmergenceInventoryItemsMetaAttribute.key;
        value = _tmpEmergenceInventoryItemsMetaAttribute.value;
    }

};

USTRUCT(BlueprintType)
struct FEmergenceInventoryItemMeta
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta")
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta")
    FString description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta")
    TArray<FEmergenceInventoryItemsMetaAttribute> attributes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta")
    TArray<FEmergenceInventoryItemsMetaContent> content;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Meta")
    FString dynamicMetadata;

    FEmergenceInventoryItemMeta() {};

    FEmergenceInventoryItemMeta(FString _name, FString _description, TArray<FEmergenceInventoryItemsMetaAttribute> _attributes, TArray<FEmergenceInventoryItemsMetaContent> _content, FString _dynamicMetadata) {

        name = _name;
        description = _description;
        attributes = _attributes;
        content = _content;
        dynamicMetadata = _dynamicMetadata;

    }

    FEmergenceInventoryItemMeta(FString _json_) {
        FEmergenceInventoryItemMeta _tmpEmergenceInventoryItemMeta;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventoryItemMeta>(
            _json_,
            &_tmpEmergenceInventoryItemMeta,
            0, 0);

        name = _tmpEmergenceInventoryItemMeta.name;
        description = _tmpEmergenceInventoryItemMeta.description;
        attributes = _tmpEmergenceInventoryItemMeta.attributes;
        content = _tmpEmergenceInventoryItemMeta.content;
        dynamicMetadata = _tmpEmergenceInventoryItemMeta.dynamicMetadata;
    }

};

USTRUCT(BlueprintType)
struct FEmergenceInventoryItemCreator
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Creator")
    FString account;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item|Creator")
    int32 value;

    FEmergenceInventoryItemCreator() {};

    FEmergenceInventoryItemCreator(FString _account, int32 _value) {

        account = _account;
        value = _value;

    }

    FEmergenceInventoryItemCreator(FString _json_) {
        FEmergenceInventoryItemCreator _tmpEmergenceInventoryItemCreator;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventoryItemCreator>(
            _json_,
            &_tmpEmergenceInventoryItemCreator,
            0, 0);

        account = _tmpEmergenceInventoryItemCreator.account;
        value = _tmpEmergenceInventoryItemCreator.value;
    }

};

USTRUCT(BlueprintType)
struct FEmergenceInventoryItem
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString blockchain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString contract;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString tokenId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    TArray<FEmergenceInventoryItemCreator> creators;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString lazySupply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString mintedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString lastUpdatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString supply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FEmergenceInventoryItemMeta meta;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    bool deleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service|Item")
    FString totalStock;

    FEmergenceInventoryItem() {};

    FEmergenceInventoryItem(FString _id, FString _blockchain, FString _contract, FString _tokenId, TArray<FEmergenceInventoryItemCreator> _creators, FString _lazySupply, FString _mintedAt, FString _lastUpdatedAt, FString _supply, FEmergenceInventoryItemMeta _meta, bool _deleted, FString _totalStock) {

        id = _id;
        blockchain = _blockchain;
        contract = _contract;
        tokenId = _tokenId;
        creators = _creators;
        lazySupply = _lazySupply;
        mintedAt = _mintedAt;
        lastUpdatedAt = _lastUpdatedAt;
        supply = _supply;
        meta = _meta;
        deleted = _deleted;
        totalStock = _totalStock;

    }

    FEmergenceInventoryItem(FString _json_) {
        FEmergenceInventoryItem _tmpEmergenceInventoryItem;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventoryItem>(
            _json_,
            &_tmpEmergenceInventoryItem,
            0, 0);

        id = _tmpEmergenceInventoryItem.id;
        blockchain = _tmpEmergenceInventoryItem.blockchain;
        contract = _tmpEmergenceInventoryItem.contract;
        tokenId = _tmpEmergenceInventoryItem.tokenId;
        creators = _tmpEmergenceInventoryItem.creators;
        lazySupply = _tmpEmergenceInventoryItem.lazySupply;
        mintedAt = _tmpEmergenceInventoryItem.mintedAt;
        lastUpdatedAt = _tmpEmergenceInventoryItem.lastUpdatedAt;
        supply = _tmpEmergenceInventoryItem.supply;
        meta = _tmpEmergenceInventoryItem.meta;
        deleted = _tmpEmergenceInventoryItem.deleted;
        totalStock = _tmpEmergenceInventoryItem.totalStock;
    }

};

USTRUCT(BlueprintType)
struct FEmergenceInventory
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Inventory Service")
    TArray<FEmergenceInventoryItem> items;

    FEmergenceInventory() {};

    FEmergenceInventory(int32 _total, TArray<FEmergenceInventoryItem> _items) {

        items = _items;

    }

    FEmergenceInventory(FString _json_) {
        FEmergenceInventory _tmpEmergenceInventory;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInventory>(
            _json_,
            &_tmpEmergenceInventory,
            0, 0);

        items = _tmpEmergenceInventory.items;
    }

};
