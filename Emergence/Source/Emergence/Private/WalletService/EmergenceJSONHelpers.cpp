// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/EmergenceJSONHelpers.h"
#include "JsonObjectConverter.h"

bool UEmergenceJSONHelpers::ReadMethodJSONToString(FJsonObjectWrapper JSONObject, FString& OutputString)
{
	FString Result;
	if (JSONObject && JSONObject.JsonObjectToString(Result)) {
		OutputString = Result;
		return true;
	}
	else {
		return false;
	}
}

bool UEmergenceJSONHelpers::ReadMethodJSONToStringArray(FJsonObjectWrapper JSONObject, TArray<FString>& OutputStringArray)
{
	OutputStringArray.Empty();
	TArray<TSharedPtr<FJsonValue>> JsonValueArray;
	if (JSONObject && JSONObject.JsonObject->HasTypedField<EJson::Array>("response")) {
		TArray<TSharedPtr<FJsonValue>> ArrayField = JSONObject.JsonObject->GetArrayField("response");
		for (int i = 0; i < ArrayField.Num(); i++) {
			FString Result;
			auto Writer = TJsonWriterFactory<>::Create(&Result);
			switch (ArrayField[i]->Type) {
				case EJson::String:
				OutputStringArray.Add(ArrayField[i]->AsString());
				break;
				case EJson::Number:
				OutputStringArray.Add(FString::FromInt(ArrayField[i]->AsNumber()));
				break;
				case EJson::Boolean:
				OutputStringArray.Add(ArrayField[i]->AsBool() ? "true" : "false");
				break;
				case EJson::Null:
				case EJson::None:
				OutputStringArray.Add("");
				break;
				case EJson::Array:
					
					if (FJsonSerializer::Serialize(ArrayField[i]->AsArray(), Writer))
					{
						OutputStringArray.Add(Result);
					}
					else {
						OutputStringArray.Add("EMERGENCE PARSE JSON ARRAY ERROR");
						return false;
					}
				break;
				case EJson::Object:
					if (FJsonSerializer::Serialize(ArrayField[i]->AsObject().ToSharedRef(), Writer))
					{
						OutputStringArray.Add(Result);
					}
					else {
						OutputStringArray.Add("EMERGENCE PARSE JSON OBJECT ERROR");
						return false;
					}
				break;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
