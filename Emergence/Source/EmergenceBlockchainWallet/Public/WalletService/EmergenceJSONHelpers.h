// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "EmergenceJSONHelpers.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UEmergenceJSONHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * Returns the ReadMethod's wrapped JSON array as a JSON string. "Return Value" is if it was a success.
	 * @param JSONObject JSONObject to turn into a string.
	 * @param OutputString The returned string.
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Helpers", meta = (DisplayName = "Read Method JSON To String"))
	static bool ReadMethodJSONToString(FJsonObjectWrapper JSONObject, FString& OutputString);

	/**
	 * Returns the ReadMethod's wrapped JSON array as a array of strings. "Return Value" is if it was a success.
	 * @param JSONObject JSONObject to turn into a string.
	 * @param OutputString The returned array of strings.
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Helpers", meta = (DisplayName = "Read Method JSON To String Array"))
	static bool ReadMethodJSONToStringArray(FJsonObjectWrapper JSONObject, TArray<FString>& OutputString);
};
