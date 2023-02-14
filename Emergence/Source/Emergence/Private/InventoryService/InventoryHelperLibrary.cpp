// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/InventoryHelperLibrary.h"

TArray<FEmergenceCombinedInventoryItem> UInventoryHelperLibrary::OrganiseInventoryItems(TArray<FEmergenceInventoryItem> InventoryItems, TArray<FEmergenceAvatarResult> Avatars)
{
    
    TArray<FEmergenceCombinedInventoryItem> ItemsWithMatchingAvatars;
    TArray<FEmergenceCombinedInventoryItem> ItemsWithNamesAndImages;
    TArray<FEmergenceCombinedInventoryItem> ItemsWithImages;
    TArray<FEmergenceCombinedInventoryItem> ItemsWithNames;

    for (int i = 0; i < InventoryItems.Num(); i++) {

       //if it doesn't have a name or an image, just skip it
       if ((InventoryItems[i].meta.name == "") && (InventoryItems[i].meta.content.Num() == 0)) {
           continue;
       }


       //Find the first (and hopefully only) avatar that matches the contract address of the current Inventory Item
       FEmergenceAvatarResult* MatchingAvatar = Avatars.FindByPredicate([&](FEmergenceAvatarResult Avatar) {
           return (Avatar.chain.ToUpper() + ":" + Avatar.contractAddress) == (InventoryItems[i].blockchain.ToUpper() + ":" + InventoryItems[i].contract);
       });

       if (MatchingAvatar) { //if we found one
           FEmergenceCombinedInventoryItem CombinedInventoryItem;
           CombinedInventoryItem.AvatarItem = *MatchingAvatar;
           CombinedInventoryItem.InventoryItem = InventoryItems[i];
           CombinedInventoryItem.HasMatchingAvatar = true;
           ItemsWithMatchingAvatars.Add(CombinedInventoryItem);
       }
       else { //if there is no matching avatar
           FEmergenceCombinedInventoryItem CombinedInventoryItem;
           CombinedInventoryItem.InventoryItem = InventoryItems[i];

           //if it has both a name and an image
           if (CombinedInventoryItem.InventoryItem.meta.name != "" && UInventoryHelperLibrary::GetBestDisplayImage(CombinedInventoryItem.InventoryItem.meta.content) != "") {
               ItemsWithNamesAndImages.Add(CombinedInventoryItem);
           }
           //if it has only an image
           else if (UInventoryHelperLibrary::GetBestDisplayImage(CombinedInventoryItem.InventoryItem.meta.content) != "") {
               ItemsWithImages.Add(CombinedInventoryItem);
           }
           // if it has only a name (ones with both names and images are filtered out earlier
           else {
               ItemsWithNames.Add(CombinedInventoryItem);
           }
       }
    }

    TArray<FEmergenceCombinedInventoryItem> OutputArray;
    OutputArray.Append(ItemsWithMatchingAvatars);
    OutputArray.Append(ItemsWithNamesAndImages);
    OutputArray.Append(ItemsWithImages);
    OutputArray.Append(ItemsWithNames);
    return OutputArray;
}

FString UInventoryHelperLibrary::GetBestDisplayImage(TArray<FEmergenceInventoryItemsMetaContent> Contents)
{
    FString BestFoundURL = "";
    for (int i = 0; i < Contents.Num(); i++) {
        
        if (Contents[i].mimeType == "image/png") { //the favourite
            return Contents[i].url;
        }
        else if (Contents[i].mimeType == "image/jpeg") { //the other option
            BestFoundURL = Contents[i].url;
        }
        else if (Contents[i].mimeType == "image/gif" && BestFoundURL == "") { //if we don't have a jpeg yet
            BestFoundURL = Contents[i].url;
        }
    }
    return BestFoundURL;
}

FString UInventoryHelperLibrary::GetBestModel(TArray<FEmergenceInventoryItemsMetaContent> Contents)
{
    FString BestFoundURL = "";
    for (int i = 0; i < Contents.Num(); i++) {

        if (Contents[i].mimeType == "model/gltf-binary") { //the favourite
            return Contents[i].url;
        }
        /*else if (Contents[i].mimeType == "image/jpeg") { //the other option
            BestFoundURL = Contents[i].url;
        }*/
    }
    return BestFoundURL;
}

TSet<FString> UInventoryHelperLibrary::GetDynamicMetadataCategories(FString DynamicMetadata)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*DynamicMetadata);

    TSet<FString> ReturnSet;

    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
        auto CategoryArray = JsonObject->GetArrayField("Categories");
        for (int i = 0; i < CategoryArray.Num(); i++) {
            ReturnSet.Add(CategoryArray[i]->AsString());
        }
    }

    return ReturnSet;
}
