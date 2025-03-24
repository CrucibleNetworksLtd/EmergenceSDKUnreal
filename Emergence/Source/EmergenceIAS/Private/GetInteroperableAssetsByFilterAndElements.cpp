// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "GetInteroperableAssetsByFilterAndElements.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "AvatarElement.h"
#include "NFTElement.h"
#include "ThumbnailsElement.h"
#include "InteroperableAsset.h"
#include "EmergenceInteroperableAssetElement.h"

UGetInteroperableAssetsByFilterAndElements* UGetInteroperableAssetsByFilterAndElements::GetInteroperableAssetsByFilterAndElements(UObject* WorldContextObject, const TSet<TSubclassOf<UEmergenceInteroperableAssetElement>>& DesiredElements, const FString& CollectionID, const FString& WalletAddress, const TArray<FString>& NFTIDs, const TArray<FString>& Blockchains, const int PageNumber, const int PageSize)
{
	UGetInteroperableAssetsByFilterAndElements* BlueprintNode = NewObject<UGetInteroperableAssetsByFilterAndElements>();
	BlueprintNode->CollectionID = FString(CollectionID);
	BlueprintNode->WalletAddress = FString(WalletAddress);
	BlueprintNode->NFTIDs = NFTIDs;
	BlueprintNode->DesiredElements = DesiredElements;
	BlueprintNode->Blockchains = Blockchains;
	BlueprintNode->PageNumber = PageNumber;
	BlueprintNode->PageSize = PageSize;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetInteroperableAssetsByFilterAndElements::Activate()
{
	FString requestURL = "https://ias-staging.openmeta.xyz/InteroperableAsset/GetByFilterAndElements";
	

	if (DesiredElements.Num() == 0 || (CollectionID.IsEmpty() && WalletAddress.IsEmpty() && NFTIDs.Num() == 0)) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("You must supply at least one Desired Element, and least one of the following, a CollectionID, a WalletAddress or an NFTID"));
		OnGetInteroperableAssetsByFilterAndElementsCompleted.Broadcast(TArray<FEmergenceInteroperableAsset>(), EErrorCode::EmergenceClientWrongType);
		return;
	}
	
	requestURL += "?iasDsrdElmnts=";
	auto ElementsAsArray = DesiredElements.Array();
	for (int i = 0; i < ElementsAsArray.Num(); i++) {
		if (!ElementsAsArray[i]) {
			continue;
		}
		
		requestURL += Cast<UEmergenceInteroperableAssetElement>(ElementsAsArray[i]->GetDefaultObject(true))->ElementName;
		//Gets the class from the array, constructs it to find its element name

		if (i + 1 != ElementsAsArray.Num()) {
			requestURL += ",";
		}
	}
	
	if (!CollectionID.IsEmpty()) {
		requestURL += "&collectionId=" + CollectionID;
	}

	if (!WalletAddress.IsEmpty()) {
		requestURL += "&walletAddress=" + WalletAddress;
	}

	if (NFTIDs.Num() > 0) {
		requestURL += "&nftIds=";
		for (int i = 0; i < NFTIDs.Num(); i++) {
			requestURL += NFTIDs[i];
			if (i + 1 != NFTIDs.Num()) {
				requestURL += ",";
			}
		}
	}

	if (Blockchains.Num() > 0) {
		requestURL += "&chains=";
		for (int i = 0; i < Blockchains.Num(); i++) {
			requestURL += Blockchains[i];
			if (i + 1 != Blockchains.Num()) {
				requestURL += ",";
			}
		}
	}

	requestURL += "&pageSize=" + FString::FromInt(PageSize);

	requestURL += "&pageNumber=" + FString::FromInt(PageNumber);
	
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Host", "interoperableassetsystem-dev.eba-xcksw4pw.us-east-1.elasticbeanstalk.com"});

	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetInteroperableAssetsByFilterAndElements>(
		this,
		&UGetInteroperableAssetsByFilterAndElements::OnGetInteroperableAssetsByFilterAndElements_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetInteroperableAssetsByFilterAndElements request started, calling OnGetInteroperableAssetsByFilterAndElements_HttpRequestComplete on request completed"));
}

void UGetInteroperableAssetsByFilterAndElements::OnGetInteroperableAssetsByFilterAndElements_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TSharedPtr<FJsonValue> JsonValue;
	TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonValue) && JsonValue.IsValid())
	{
		auto JsonAsIAArray = JsonValue->AsArray();

		TArray<FEmergenceInteroperableAsset> OutputIAs;
		for (auto JsonAsIAArrayMember : JsonAsIAArray)
		{
			auto IAObject = JsonAsIAArrayMember->AsObject();
			FEmergenceInteroperableAsset OutputIA;
			OutputIA.Id = IAObject->GetStringField("Id");
			auto ElementsArray = IAObject->GetArrayField("Elements");
			for (auto ElementsArrayMember : ElementsArray) {
				auto ElementObject = ElementsArrayMember->AsObject();
				
				FString ElementObjectAsString;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ElementObjectAsString);
				FJsonSerializer::Serialize(ElementObject.ToSharedRef(), Writer);

				FString ElementType = ElementObject->GetStringField("ElementName");
				if (ElementType == "NFT") {
					auto NFTElement = NewObject<UNFTElement>(WorldContextObject);
					NFTElement->NFTElementData = FNFTElementInner(ElementObjectAsString);
					OutputIA.Elements.Add(NFTElement);
				}
				if (ElementType == "avatar") {
					auto AvatarElement = NewObject<UAvatarElement>(WorldContextObject);
					AvatarElement->AvatarElementData = FAvatarElementInner(ElementObjectAsString);
					OutputIA.Elements.Add(AvatarElement);
				}
				if (ElementType == "thumbnails") {
					auto ThumbnailsElement = NewObject<UThumbnailsElement>(WorldContextObject);
					ThumbnailsElement->ThumbnailsElementData = FThumbnailsElementInner(ElementObjectAsString);
					OutputIA.Elements.Add(ThumbnailsElement);
				}

			}
			OutputIAs.Add(OutputIA);
		}
		OnGetInteroperableAssetsByFilterAndElementsCompleted.Broadcast(OutputIAs, EErrorCode::EmergenceOk);
	}
	else {
		OnGetInteroperableAssetsByFilterAndElementsCompleted.Broadcast(TArray<FEmergenceInteroperableAsset>(), EErrorCode::EmergenceClientJsonParseFailed);
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpRequest->GetURL());
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
	
	
	
	
	
	
	/*EErrorCode StatusCode;
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode == EErrorCode::EmergenceOk) {
		

		//OnGetInteroperableAssetsByFilterAndElementsCompleted.Broadcast(TArray<FEmergenceInteroperableAsset(HttpResponse->GetContentAsString())>, StatusCode);
	}
	else {
		//OnGetInteroperableAssetsByFilterAndElementsCompleted.Broadcast(FEmergenceInteroperableAsset(), StatusCode);
	}*/
	SetReadyToDestroy();
}