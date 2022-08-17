// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/EmergenceScreen.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "InventoryService/InventoryHelperLibrary.h"
#include "InventoryScreen.generated.h"

UENUM(BlueprintType)
enum class EEmergenceInventoryBlockchain : uint8 {
    Any = 0,
    Ethereum = 1,
    Polygon = 2,
    Flow = 3,
    Tezos = 4,
    Solana = 5,
    Immutablex = 6
};

USTRUCT(BlueprintType)
struct FEmergenceInventoryFilterSetCategories
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Avatars = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Props = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Clothing = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Weapons = true;
};

USTRUCT(BlueprintType)
struct FEmergenceInventoryFilterSet
{

    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "UseSearch"))
    FString Search = "";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowInnerProperties, EditCondition = "UseCategories"))
    FEmergenceInventoryFilterSetCategories Categories;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "UseBlockchain"))
    EEmergenceInventoryBlockchain Blockchain;

};

UCLASS()
class UInventoryScreen : public UEmergenceScreen
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FEmergenceInventoryFilterSet Filters;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, FEmergenceCombinedInventoryItem, Item);

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnItemSelected OnItemSelected;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (ExposeOnSpawn = true))
    FString Address = "0xc33411F5dAE18253AB23068B700B5a0c9C44DA2C";

    //Shows a message on the inventory to inform the user that the inventory they are view is not theirs
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (ExposeOnSpawn = true))
    bool ShowExternalInventoryMessage = false;
};
