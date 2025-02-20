// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "Types/EmergenceErrorCode.h"
#include "Interfaces/IHttpResponse.h"

const TMap <int32, EErrorCode> UEmergenceErrorCode::StatusCodeIntToErrorCode = {
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
	{-6, EErrorCode::EmergenceClientUserRejected},
	{512, EErrorCode::EmergenceInternalError}
};

EErrorCode UEmergenceErrorCode::Conv_IntToErrorCode(int32 Status)
{
	const EErrorCode* ErrorCode = UEmergenceErrorCode::StatusCodeIntToErrorCode.Find(Status);
	if (ErrorCode != nullptr) {
		return *ErrorCode;
	}
	else {
		return EErrorCode::EmergenceClientInvalidErrorCode;
	}
}

int32 UEmergenceErrorCode::Conv_ErrorCodeToInt(EErrorCode ErrorCode)
{
	auto Int = UEmergenceErrorCode::StatusCodeIntToErrorCode.FindKey(ErrorCode);
	if (Int != nullptr) {
		return *Int;
	}
	else {
		return (int32)EErrorCode::EmergenceClientInvalidErrorCode;
	}
}

bool UEmergenceErrorCode::Conv_ErrorCodeToBool(EErrorCode ErrorCode)
{
	return ErrorCode == EErrorCode::EmergenceOk;
}

