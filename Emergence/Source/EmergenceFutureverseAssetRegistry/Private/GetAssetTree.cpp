// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "GetAssetTree.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Types/EmergenceErrorCode.h"
#include "Interfaces/IHttpResponse.h"
#include "EmergenceFutureverseAssetRegistry.h"

UGetFutureverseAssetTree* UGetFutureverseAssetTree::GetFutureverseAssetTree(UObject* WorldContextObject, FString TokenId, FString CollectionId)
{
	UGetFutureverseAssetTree* BlueprintNode = NewObject<UGetFutureverseAssetTree>();
	BlueprintNode->TokenId = TokenId;
	BlueprintNode->CollectionId = CollectionId;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetFutureverseAssetTree::Activate()
{
	if (!TokenId.IsEmpty() && !CollectionId.IsEmpty()) {
		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>{ "content-type", "application/json" });
		
		FString Content = R"({"query":"query Asset($tokenId: String!, $collectionId: CollectionId!) {\n  asset(tokenId: $tokenId, collectionId: $collectionId) {\n    assetTree {\n      data\n    }\n  }\n}","variables":{"tokenId":")" + TokenId + R"(","collectionId":")" + CollectionId + R"("}})";
		Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetFutureverseAssetTree>(
			this,
			&UGetFutureverseAssetTree::GetAssetTree_HttpRequestComplete,
			FEmergenceFutureverseAssetRegistryModule::GetFutureverseAssetRegistryAPIURL(),
			"POST",
			60.0F,
			Headers,
			Content);
		UE_LOG(LogEmergenceHttp, Display, TEXT("UGetAssetTree request started, calling UGetAssetTree_HttpRequestComplete on request completed."));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("One or more of UGetAssetTree's inputs were empty."));
		OnGetAssetTreeCompleted.Broadcast(TArray<FFutureverseAssetTreePath>(), EErrorCode::EmergenceClientFailed);
	}
}

void UGetFutureverseAssetTree::GetAssetTree_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetAssetTree_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());

	if (StatusCode == EErrorCode::EmergenceOk) {
		TSharedPtr<FJsonValue> JsonValue;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TSharedPtr<FJsonObject> Object = JsonValue->AsObject();
			if (Object) {
				auto DataArray = Object->GetObjectField("data")->GetObjectField("asset")->GetObjectField("assetTree")->GetObjectField("data")->GetArrayField("@graph");
				TArray<FFutureverseAssetTreePath> AssetTree;

				for (auto Data : DataArray) {
					FFutureverseAssetTreePath AssetTreePartStruct;

					//so as of recently some assets will have predicates that don't have IDs nor RdfTypes, so we can't assume they'll exist any more
					Data->AsObject()->TryGetStringField("@id", AssetTreePartStruct.Id); //get the ID of this, fail nicely if it doesn't exist

					TSharedPtr<FJsonValue> RdfTypeObject = Data->AsObject()->TryGetField("rdf:type");
					if (RdfTypeObject && RdfTypeObject->Type == EJson::Object) { //if it isn't an object, its probably null, so no need to handle any other type
						RdfTypeObject->AsObject()->TryGetStringField("@id", AssetTreePartStruct.RDFType); //get the rdf:type - if it somehow doesn't have an ID, handle this nicely too
					}
					
					TArray<FString> PredicateKeys;
					if (Data->AsObject()->Values.GetKeys(PredicateKeys) > 0) {
						for (FString PredicateKey : PredicateKeys) {

							if (PredicateKey == "@id" || PredicateKey == "rdf:type") { //ignore these ones
								continue;
							}

							FFutureverseAssetTreeObject PredicateStruct;
							TSharedPtr<FJsonObject> Predicate = Data->AsObject()->GetObjectField(PredicateKey);
							PredicateStruct.Id = Predicate->GetStringField("@id");

							TArray<FString> PredicateContentKeys;
							if (Predicate->Values.GetKeys(PredicateContentKeys) > 0) {
								for (FString PredicateContentKey : PredicateContentKeys) {
									if (PredicateContentKey == "@id") { //ignore
										continue;
									}

									FString JsonAsString;
									TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonAsString);
									FJsonSerializer::Serialize(*Predicate->Values.Find(PredicateContentKey), PredicateContentKey, Writer);

									PredicateStruct.AdditionalData.Add(PredicateContentKey, JsonAsString);
								}
							}
							
							AssetTreePartStruct.Objects.Add(PredicateKey, PredicateStruct);
							
						}
					}
					AssetTree.Add(AssetTreePartStruct);
				}
				OnGetAssetTreeCompleted.Broadcast(AssetTree, EErrorCode::EmergenceOk);
				return;
			}
		}
	}
	OnGetAssetTreeCompleted.Broadcast(TArray<FFutureverseAssetTreePath>(), EErrorCode::EmergenceClientFailed);
	UE_LOG(LogEmergenceHttp, Error, TEXT("One or more errors occured trying to parse asset tree."));
	return;
}