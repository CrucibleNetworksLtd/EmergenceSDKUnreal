// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Emergence.h"
#include "Dom/JsonObject.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "ErrorCodeFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EErrorCode : uint8 {
	EmergenceOk = 0,
	EmergenceNotConnected = 1,
	EmergenceAlreadyConnected = 2,
	EmergenceInternalError = 3,

	//HTTP STATUES (can't go from http error code straight to this enum, use converter)
	Unknown = 100,
	Continue = 101,
	SwitchProtocol = 102,
	Ok = 103,
	Created = 104,
	Accepted = 105,
	Partial = 106,
	NoContent = 107,
	ResetContent = 108,
	PartialContent = 109,
	Ambiguous = 110,
	Moved = 111,
	Redirect = 112,
	RedirectMethod = 113,
	NotModified = 114,
	UseProxy = 115,
	RedirectKeepVerb = 116,
	BadRequest = 117,
	Denied = 118,
	PaymentReq = 119,
	Forbidden = 120,
	NotFound = 121,
	BadMethod = 122,
	NoneAcceptable = 123,
	ProxyAuthReq = 124,
	RequestTimeout = 125,
	Conflict = 126,
	Gone = 127,
	LengthRequired = 128,
	PrecondFailed = 129,
	RequestTooLarge = 130,
	UriTooLong = 131,
	UnsupportedMedia = 132,
	TooManyRequests = 133,
	RetryWith = 134,
	ServerError = 135,
	NotSupported = 136,
	BadGateway = 137,
	ServiceUnavail = 138,
	GatewayTimeout = 139,
	VersionNotSup = 140,

	//Unreal Emergence-sided errors
	EmergenceClientFailed = 201,
	EmergenceClientInvalidResponse = 202,
	EmergenceClientInvalidErrorCode = 203,
	EmergenceClientJsonParseFailed = 204,
	EmergenceClientWrongType = 205
};

UCLASS()
class EMERGENCE_API UErrorCodeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//tries to parse a response as json, calls GetResponseErrors automagically
	static FJsonObject TryParseResponseAsJson(FHttpResponsePtr HttpResponse, bool bSucceeded, EErrorCode& JsonObject);

	//Gets any pre-content parse errors
	static EErrorCode GetResponseErrors(FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION(BlueprintPure, Category="Emergence|Error Code Helpers", meta = (DisplayName = "Status Code Int To Status Code Enum", CompactNodeTitle = "->", BlueprintAutocast))
	static EErrorCode Conv_IntToErrorCode(int32 HttpStatus);

	UFUNCTION(BlueprintPure, Category="Emergence|Error Code Helpers", meta = (DisplayName = "Status Code Enum to Status Code Int", CompactNodeTitle = "->", BlueprintAutocast))
	static int32 Conv_ErrorCodeToInt(EErrorCode ErrorCode);

	UFUNCTION(BlueprintPure, Category="Emergence|Error Code Helpers", meta = (DisplayName = "Status Code Enum to Status Ok Bool", CompactNodeTitle = "->", BlueprintAutocast))
	static bool Conv_ErrorCodeToBool(EErrorCode ErrorCode);

	static const TMap<int32, TEnumAsByte<EErrorCode>> StatusCodeIntToErrorCode;
};
