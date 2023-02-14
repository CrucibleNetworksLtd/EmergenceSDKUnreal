// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "ErrorCodeFunctionLibrary.h"
#include "Interfaces/IHttpResponse.h"
#pragma warning( push )
#pragma warning( disable : 4996 )
const TMap <int32, TEnumAsByte<EErrorCode>> UErrorCodeFunctionLibrary::StatusCodeIntToErrorCode = {
	{(int32)EErrorCode::EmergenceOk, EErrorCode::EmergenceOk},
	{(int32)EErrorCode::EmergenceNotConnected, EErrorCode::EmergenceNotConnected},
	{(int32)EErrorCode::EmergenceAlreadyConnected, EErrorCode::EmergenceAlreadyConnected},
	{(int32)EHttpResponseCodes::Continue, EErrorCode::Continue},
	{(int32)EHttpResponseCodes::SwitchProtocol, EErrorCode::SwitchProtocol},
	{(int32)EHttpResponseCodes::Ok, EErrorCode::Ok},
	{(int32)EHttpResponseCodes::Created, EErrorCode::Created},
	{(int32)EHttpResponseCodes::Accepted, EErrorCode::Accepted},
	{(int32)EHttpResponseCodes::Partial, EErrorCode::Partial},
	{(int32)EHttpResponseCodes::NoContent, EErrorCode::NoContent},
	{(int32)EHttpResponseCodes::ResetContent, EErrorCode::ResetContent},
	{(int32)EHttpResponseCodes::PartialContent, EErrorCode::PartialContent},
	{(int32)EHttpResponseCodes::Ambiguous, EErrorCode::Ambiguous},
	{(int32)EHttpResponseCodes::Moved, EErrorCode::Moved},
	{(int32)EHttpResponseCodes::Redirect, EErrorCode::Redirect},
	{(int32)EHttpResponseCodes::RedirectMethod, EErrorCode::RedirectMethod},
	{(int32)EHttpResponseCodes::NotModified, EErrorCode::NotModified},
	{(int32)EHttpResponseCodes::UseProxy, EErrorCode::UseProxy},
	{(int32)EHttpResponseCodes::RedirectKeepVerb, EErrorCode::RedirectKeepVerb},
	{(int32)EHttpResponseCodes::BadRequest, EErrorCode::BadRequest},
	{(int32)EHttpResponseCodes::Denied, EErrorCode::Denied},
	{(int32)EHttpResponseCodes::PaymentReq, EErrorCode::PaymentReq},
	{(int32)EHttpResponseCodes::Forbidden, EErrorCode::Forbidden},
	{(int32)EHttpResponseCodes::NotFound, EErrorCode::NotFound},
	{(int32)EHttpResponseCodes::BadMethod, EErrorCode::BadMethod},
	{(int32)EHttpResponseCodes::NoneAcceptable, EErrorCode::NoneAcceptable},
	{(int32)EHttpResponseCodes::ProxyAuthReq, EErrorCode::ProxyAuthReq},
	{(int32)EHttpResponseCodes::RequestTimeout, EErrorCode::RequestTimeout},
	{(int32)EHttpResponseCodes::Conflict, EErrorCode::Conflict},
	{(int32)EHttpResponseCodes::Gone, EErrorCode::Gone},
	{(int32)EHttpResponseCodes::LengthRequired, EErrorCode::LengthRequired},
	{(int32)EHttpResponseCodes::PrecondFailed, EErrorCode::PrecondFailed},
	{(int32)EHttpResponseCodes::RequestTooLarge, EErrorCode::RequestTooLarge},
	{(int32)EHttpResponseCodes::UriTooLong, EErrorCode::UriTooLong},
	{(int32)EHttpResponseCodes::UnsupportedMedia, EErrorCode::UnsupportedMedia},
	{(int32)EHttpResponseCodes::TooManyRequests, EErrorCode::TooManyRequests},
	{(int32)EHttpResponseCodes::RetryWith, EErrorCode::RetryWith},
	{(int32)EHttpResponseCodes::ServerError, EErrorCode::ServerError},
	{(int32)EHttpResponseCodes::NotSupported, EErrorCode::NotSupported},
	{(int32)EHttpResponseCodes::BadGateway, EErrorCode::BadGateway},
	{(int32)EHttpResponseCodes::ServiceUnavail, EErrorCode::ServiceUnavail},
	{(int32)EHttpResponseCodes::GatewayTimeout, EErrorCode::GatewayTimeout},
	{(int32)EHttpResponseCodes::VersionNotSup, EErrorCode::VersionNotSup},
	{-1, EErrorCode::EmergenceClientFailed},
	{-2, EErrorCode::EmergenceClientInvalidResponse},
	{-3, EErrorCode::EmergenceClientInvalidErrorCode},
	{-4, EErrorCode::EmergenceClientJsonParseFailed},
	{-5, EErrorCode::EmergenceClientWrongType},
	{512, EErrorCode::EmergenceInternalError}
};
#pragma warning( pop )

FJsonObject UErrorCodeFunctionLibrary::TryParseResponseAsJson(FHttpResponsePtr HttpResponse, bool bSucceeded, EErrorCode& ReturnResponseCode) {

	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(ResponseCode))) {
		ReturnResponseCode = ResponseCode;
		return FJsonObject();
	}

	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->HasField("statusCode"))
		{
			ReturnResponseCode = UErrorCodeFunctionLibrary::Conv_IntToErrorCode(JsonObject->GetIntegerField("statusCode"));
		}
		else {
			//this fixes weird behaviour with GetPersonas
			ReturnResponseCode = EErrorCode::EmergenceOk;
		}
		return *JsonObject.Get();
	}
	ReturnResponseCode = EErrorCode::EmergenceClientJsonParseFailed;
	return FJsonObject();
}

EErrorCode UErrorCodeFunctionLibrary::GetResponseErrors(FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	//If we didn't even get a http response, give failed
	if (!bSucceeded) return EErrorCode::EmergenceClientFailed;

	//if we got one but its not readable, give invalid response
	if (!HttpResponse.IsValid()) return EErrorCode::EmergenceClientInvalidResponse;

	//if we got a readable one but it has a http error, give that
	if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) {
		UE_LOG(LogEmergenceHttp, Warning,TEXT("%s"),*HttpResponse->GetContentAsString());
	}
	return UErrorCodeFunctionLibrary::Conv_IntToErrorCode(HttpResponse->GetResponseCode());
}

EErrorCode UErrorCodeFunctionLibrary::Conv_IntToErrorCode(int32 Status)
{
	auto ErrorCode = UErrorCodeFunctionLibrary::StatusCodeIntToErrorCode.Find(Status);
	if (ErrorCode != nullptr) {
		return *ErrorCode;
	}
	else {
		return EErrorCode::EmergenceClientInvalidErrorCode;
	}
}

int32 UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(EErrorCode ErrorCode)
{
	auto Int = UErrorCodeFunctionLibrary::StatusCodeIntToErrorCode.FindKey(ErrorCode);
	if (Int != nullptr) {
		return *Int;
	}
	else {
		return (int32)EErrorCode::EmergenceClientInvalidErrorCode;
	}
}

bool UErrorCodeFunctionLibrary::Conv_ErrorCodeToBool(EErrorCode ErrorCode)
{
	return ErrorCode == EErrorCode::EmergenceOk;
}

