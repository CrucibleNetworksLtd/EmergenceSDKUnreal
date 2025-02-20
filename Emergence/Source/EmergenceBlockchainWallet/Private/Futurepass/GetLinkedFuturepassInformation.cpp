// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "Futurepass/GetLinkedFuturepassInformation.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "FutureverseEnvironmentLibrary.h"

UGetLinkedFuturepassInformation* UGetLinkedFuturepassInformation::GetLinkedFuturepassInformation(UObject* WorldContextObject, FString Address)
{
	UGetLinkedFuturepassInformation* BlueprintNode = NewObject<UGetLinkedFuturepassInformation>();
	BlueprintNode->Address = Address;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetLinkedFuturepassInformation::Activate(){
	FString URL = UFutureverseEnvironmentLibrary::GetFutureverseFuturepassAPIURL() + "/linked-futurepass?eoa=";
	URL = URL + UFutureverseEnvironmentLibrary::GetFutureverseFuturepassChainId() + ":EVM:" + Address;
	LinkedFuturepassRequest = UHttpHelperLibrary::ExecuteHttpRequest<UGetLinkedFuturepassInformation>(
		this,
		nullptr,
		URL,
		"GET",
		60.0F,
		{},
		"", false);

	LinkedFuturepassRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		EErrorCode StatusCode;
		FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(req, res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetLinkedFuturepassInformation part 1: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk) {	
			TSharedPtr<FJsonValue> JsonValue;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
				FString OwnedFuturepass;
				if(JsonValue->AsObject()->TryGetStringField("ownedFuturepass", OwnedFuturepass)){
					FString LinkedEOAURL = UFutureverseEnvironmentLibrary::GetFutureverseFuturepassAPIURL() + "/linked-eoa?futurepass=" + OwnedFuturepass;
					
					LinkedEOARequest = UHttpHelperLibrary::ExecuteHttpRequest<UGetLinkedFuturepassInformation>(
						this,
						nullptr,
						LinkedEOAURL,
						"GET",
						60.0F,
						{},
						"", false);
					LinkedEOARequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded2) {
						UE_LOG(LogEmergenceHttp, Display, TEXT("GetLinkedFuturepassInformation part 2: %s"), *res->GetContentAsString());
						FLinkedFuturepassInformationResponse Response = FLinkedFuturepassInformationResponse(res->GetContentAsString());
						this->OnGetLinkedFuturepassInformationCompleted.Broadcast(Response, bSucceeded2 ? EErrorCode::EmergenceOk : EErrorCode::EmergenceInternalError);
						return;
					});
					LinkedEOARequest->ProcessRequest();
					
				}
				else if (JsonValue->AsObject()->HasTypedField<EJson::Null>("ownedFuturepass")) {
					//this happens when the owned futurepass doesn't exist. Leave the response blank, but give an error.
					this->OnGetLinkedFuturepassInformationCompleted.Broadcast(FLinkedFuturepassInformationResponse(), EErrorCode::Forbidden);
					return;
				}
			}
			else {
				//Json deserialize failed, override error message
				this->OnGetLinkedFuturepassInformationCompleted.Broadcast(FLinkedFuturepassInformationResponse(), EErrorCode::EmergenceClientJsonParseFailed);
				return;
			}
		}
		else {
			//A HTTP error happened, actually use status code
			this->OnGetLinkedFuturepassInformationCompleted.Broadcast(FLinkedFuturepassInformationResponse(), StatusCode);
			return;
		}
		});
	LinkedFuturepassRequest->ProcessRequest();
}

void UGetLinkedFuturepassInformation::Cancel()
{
	if (LinkedFuturepassRequest) {
		LinkedFuturepassRequest->OnProcessRequestComplete().Unbind();
		LinkedFuturepassRequest->CancelRequest();
	}
	if (LinkedEOARequest) {
		LinkedEOARequest->OnProcessRequestComplete().Unbind();
		LinkedEOARequest->CancelRequest();
	}
}

bool UGetLinkedFuturepassInformation::IsActive() const
{
	return LinkedFuturepassRequest->GetStatus() == EHttpRequestStatus::Processing ||
		LinkedEOARequest->GetStatus() == EHttpRequestStatus::Processing;
}

void UGetLinkedFuturepassInformation::GetLinkedFuturepassInformation_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded){
	
}