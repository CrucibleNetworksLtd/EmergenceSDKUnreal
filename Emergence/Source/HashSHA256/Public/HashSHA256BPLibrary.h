// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SHA256Hash.h"
#include "HashSHA256BPLibrary.generated.h"

/**
 *
 */
UCLASS()
class HASHSHA256_API UHashSHA256BPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SHA256Hash|Methods")
	static void FromString(UPARAM(ref) FSHA256Hash& SHA256, const FString &Str) { SHA256.FromString(Str); }

	UFUNCTION(BlueprintCallable, Category = "SHA256Hash|Methods")
	static void FromFile(UPARAM(ref) FSHA256Hash& SHA256, const FString& Filename) { SHA256.FromFile(Filename); }

	UFUNCTION(BlueprintCallable, Category = "SHA256Hash|Methods")
	static void FromArray(UPARAM(ref) FSHA256Hash& SHA256, const TArray<uint8>& Arr) { SHA256.FromArray(Arr); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SHA256Hash|Methods")
	static FString GetHash(UPARAM(ref) FSHA256Hash& SHA256) { return SHA256.GetHash(); }
};
