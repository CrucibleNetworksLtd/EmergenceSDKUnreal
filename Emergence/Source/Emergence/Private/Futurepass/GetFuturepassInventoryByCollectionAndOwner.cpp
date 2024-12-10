// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Futurepass/GetFuturepassInventoryByCollectionAndOwner.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetFuturepassInventoryByCollectionAndOwner* UGetFuturepassInventoryByCollectionAndOwner::GetFuturepassInventoryByCollectionAndOwner(UObject* WorldContextObject, TArray<FString> Addresses, TArray<FString> Collections){
	UGetFuturepassInventoryByCollectionAndOwner* BlueprintNode = NewObject<UGetFuturepassInventoryByCollectionAndOwner>();
	BlueprintNode->Addresses = Addresses;
	BlueprintNode->Collections = Collections;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetFuturepassInventoryByCollectionAndOwner::Activate() {
	FString AddressString;
	for (int i = 0; i < Addresses.Num(); i++) {
		if (i != 0) {
			AddressString = AddressString + ",";
		}
		AddressString = AddressString + "\"" + Addresses[i] + "\"";
	}
	
	FString CollectionString;
	for (int i = 0; i < Collections.Num(); i++) {
		if (i != 0) {
			CollectionString = CollectionString + ",";
		}
		CollectionString = CollectionString + "\"" + Collections[i] + "\"";
	}


	FString Content = R"({"query":"query Asset($addresses: [ChainAddress!]!, $first: Float, $collectionIds: [CollectionId!]) {\r\n  assets(addresses: $addresses, first: $first, collectionIds: $collectionIds) {\r\n    edges {\r\n      node {\r\n        metadata {\r\n          properties\r\n          attributes\r\n        rawAttributes\r\n}\r\n        collection {\r\n          chainId\r\n          chainType\r\n          location\r\n          name\r\n        }\r\ntokenId\r\ncollectionId      }\r\n    }\r\n  }\r\n}","variables":{"addresses":[)" + AddressString + R"(], "first": 1000, "collectionIds":[)" + CollectionString + R"(] }})";
	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetFuturepassInventoryByCollectionAndOwner>(
		this,
		nullptr,
		UHttpHelperLibrary::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		60.0F, //give the user lots of time to mess around setting high gas fees
		{TPair<FString, FString>("Content-Type","application/json")},
		Content, false);
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		//Yes, I know this is WET code when you consider its already implemented in GetFuturepassInventory. 
		//However, given that we're going to probably be changing this up a bit relatively soon when we implement IAS, I think its worth the time saving now so that...
		//...FV can start to make use of this in their games now. Just keep in mind when this and GetFuturepassInventory is restructured, 
		//that this bit of code below is pretty much the same as that in GetFuturepassInventory.
		
		EErrorCode StatusCode;
		FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetFuturepassInventory_HttpRequestComplete: %s"), *res->GetContentAsString());

		if (StatusCode == EErrorCode::EmergenceOk) {
			TSharedPtr<FJsonValue> JsonValue;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
				TSharedPtr<FJsonObject> Object = JsonValue->AsObject();
				if (Object) {
					auto NFTArray = Object->GetObjectField("data")->GetObjectField("assets")->GetArrayField("edges");
					TArray<FEmergenceInventoryItem> Items;

					for (auto NFTNode : NFTArray) {
						auto NFTData = NFTNode->AsObject()->GetObjectField("node");
						
						auto Collection = NFTData->GetObjectField("collection");

						FEmergenceInventoryItem Item;
						
						FString TokenIDString = NFTData->GetStringField("tokenId");
						FString ContractAddress = Collection->GetStringField("location");
						FString ChainType = Collection->GetStringField("chainType");
						FString ChainId = Collection->GetStringField("chainId");
						Item.id = NFTData->GetStringField("collectionId") + ":" + TokenIDString;
						Item.contract = ContractAddress;
						Item.tokenId = TokenIDString;
						Item.blockchain = ChainId + ":" + ChainType;
						
						//Parse properties
						const TSharedPtr<FJsonObject>* Properties;
						if (NFTData->GetObjectField("metadata")->TryGetObjectField("properties", Properties)) {
							if (Properties->Get()->HasTypedField<EJson::String>("name")) {
								Item.meta.name = Properties->Get()->GetStringField("name");
							}
							else {
								Item.meta.name = "#" + TokenIDString;
							}
							Properties->Get()->TryGetStringField("description", Item.meta.description);

							//Parse content (specifically the image field)
							FEmergenceInventoryItemsMetaContent Content;
							Content.url = Properties->Get()->GetStringField("image");
							if (Content.url.Contains(".png")) {
								Content.mimeType = "image/png";
								Item.meta.content.Add(Content);
							}
							else if (Content.url.Contains(".jpg") || Content.url.Contains(".jpeg")) {
								Content.mimeType = "image/jpeg";
								Item.meta.content.Add(Content);
							}
							else if (Content.url.Contains(".gif")) {
								Content.mimeType = "image/gif";
								Item.meta.content.Add(Content);
							}
						}

						//Parse attributes
						const TSharedPtr<FJsonObject>* Attributes;
						if (NFTData->GetObjectField("metadata")->TryGetObjectField("attributes", Attributes)) {
							for (TPair<FString, TSharedPtr<FJsonValue>> Attribute : Attributes->Get()->Values) {
								FString JsonContentString;
								FEmergenceInventoryItemsMetaAttribute AttributeStruct;
								AttributeStruct.key = Attribute.Key;
								if (Attribute.Value->TryGetString(AttributeStruct.value)) {
									Item.meta.attributes.Add(AttributeStruct);
								}
								else {
									UE_LOG(LogEmergence, Warning, TEXT("Couldn't parse attribute %s as part of GetFuturepassInventory."), *Attribute.Key);
								}
							}
						}

						Item.meta.attributes.Add(FEmergenceInventoryItemsMetaAttribute("CollectionName", Collection->GetStringField("name")));

						//add the entire item to our array
						Item.OriginalData.JsonObject = NFTNode->AsObject();
						Items.Add(Item);
					}

					FEmergenceInventory Inventory = FEmergenceInventory(Items.Num(), Items);
					this->OnGetFuturepassInventoryCompleted.Broadcast(Inventory, StatusCode);
				}
			}
		}
		else {
			this->OnGetFuturepassInventoryCompleted.Broadcast(FEmergenceInventory(), StatusCode);
			return;
		}
	});
	Request->ProcessRequest();
}