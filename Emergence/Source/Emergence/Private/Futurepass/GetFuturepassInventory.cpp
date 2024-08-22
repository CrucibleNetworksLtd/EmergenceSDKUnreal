// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Futurepass/GetFuturepassInventory.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetFuturepassInventory* UGetFuturepassInventory::GetFuturepassInventory(UObject* WorldContextObject, TArray<FString> Addresses){
	UGetFuturepassInventory* BlueprintNode = NewObject<UGetFuturepassInventory>();
	BlueprintNode->Addresses = Addresses;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetFuturepassInventory::Activate() {
	FString AddressString;
	for (int i = 0; i < Addresses.Num(); i++) {
		if (i != 0) {
			AddressString = AddressString + ",";
		}
		AddressString = AddressString + "\"" + Addresses[i] + "\"";
	}

	FString Content = R"({"query":"query Asset($addresses: [ChainAddress!]!, $first: Float) {\r\n  assets(addresses: $addresses, first: $first) {\r\n    edges {\r\n      node {\r\n        metadata {\r\n          properties\r\n          attributes\r\n        }\r\n        collection {\r\n          chainId\r\n          chainType\r\n          location\r\n          name\r\n        }\r\ntokenId\r\ncollectionId      }\r\n    }\r\n  }\r\n}","variables":{"addresses":[)" + AddressString + R"(], "first": 1000 }})";
	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetFuturepassInventory>(
		this,
		nullptr,
		UHttpHelperLibrary::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		60.0F, //give the user lots of time to mess around setting high gas fees
		{TPair<FString, FString>("Content-Type","application/json")},
		Content, false);
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
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