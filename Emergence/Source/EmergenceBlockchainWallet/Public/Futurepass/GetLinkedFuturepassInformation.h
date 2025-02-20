// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "JsonObjectConverter.h"
#include "GetLinkedFuturepassInformation.generated.h"

USTRUCT(BlueprintType)
struct FLinkedFuturepassEOA
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	int proxyType = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	FString eoa;

	FLinkedFuturepassEOA() {};

	FLinkedFuturepassEOA(int _proxyType, FString _eoa) {

		proxyType = _proxyType;
		eoa = _eoa;
	}

	FLinkedFuturepassEOA(FString _json_) {
		FLinkedFuturepassEOA _tmpLinkedFuturepassEOA;

		FJsonObjectConverter::JsonObjectStringToUStruct<FLinkedFuturepassEOA>(
			_json_,
			&_tmpLinkedFuturepassEOA,
			0, 0);

		proxyType = _tmpLinkedFuturepassEOA.proxyType;
		eoa = _tmpLinkedFuturepassEOA.eoa;
	};

};

USTRUCT(BlueprintType)
struct FLinkedFuturepassInformationResponse
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	FString futurepass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	FString ownerEoa;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	TArray<FLinkedFuturepassEOA> linkedEoas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse")
	TArray<FLinkedFuturepassEOA> invalidEoas;

	FLinkedFuturepassInformationResponse() {};

	FLinkedFuturepassInformationResponse(FString _futurepass, FString _ownerEoa, TArray<FLinkedFuturepassEOA> _linkedEoas, TArray<FLinkedFuturepassEOA> _invalidEoas) {

		futurepass = _futurepass;
		ownerEoa = _ownerEoa;
		linkedEoas = _linkedEoas;
		invalidEoas = _invalidEoas;
	}

	FLinkedFuturepassInformationResponse(FString _json_) {
		FLinkedFuturepassInformationResponse _tmpLinkedFuturepassInformationResponse;

		FJsonObjectConverter::JsonObjectStringToUStruct<FLinkedFuturepassInformationResponse>(
			_json_,
			&_tmpLinkedFuturepassInformationResponse,
			0, 0);

		futurepass = _tmpLinkedFuturepassInformationResponse.futurepass;
		ownerEoa = _tmpLinkedFuturepassInformationResponse.ownerEoa;
		linkedEoas = _tmpLinkedFuturepassInformationResponse.linkedEoas;
		invalidEoas = _tmpLinkedFuturepassInformationResponse.invalidEoas;
	};

};


UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UGetLinkedFuturepassInformation : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Given a wallet address, gets any linked futurepass information relevent for this wallet.
	 * @param Address The address to find a linked futurepass for. This is usually the user's Ethereum address.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse|Futurepass")
	static UGetLinkedFuturepassInformation* GetLinkedFuturepassInformation(UObject* WorldContextObject, FString Address);

	virtual void Activate() override;

	virtual void Cancel();

	virtual bool IsActive() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetLinkedFuturepassInformationCompleted, FLinkedFuturepassInformationResponse, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetLinkedFuturepassInformationCompleted OnGetLinkedFuturepassInformationCompleted;
private:
	void GetLinkedFuturepassInformation_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> LinkedFuturepassRequest;
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> LinkedEOARequest;

	FString Address;
};
