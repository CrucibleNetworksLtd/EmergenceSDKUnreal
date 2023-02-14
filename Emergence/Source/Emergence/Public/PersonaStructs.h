// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "PersonaStructs.generated.h"

USTRUCT(BlueprintType)
struct FEmergencePersona
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona")
	FString id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona")
	FString bio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona")
	FString avatarId;

	FEmergencePersona() {};

	FEmergencePersona( FString _id, FString _name, FString _bio, FString _avatarId){

		id = _id;
		name = _name;
		bio = _bio;
		avatarId = _avatarId;
	}
	
	FEmergencePersona(FString _json_){
		FEmergencePersona _tmpEmergencePersona;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersona>(
		_json_,
		&_tmpEmergencePersona,
		0, 0);
		
		id = _tmpEmergencePersona.id;
		name = _tmpEmergencePersona.name;
		bio = _tmpEmergencePersona.bio;
		avatarId = _tmpEmergencePersona.avatarId;
	}
	
};

USTRUCT(BlueprintType)
struct FEmergencePersonaListResponse
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona List")
	TArray<FEmergencePersona> personas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Persona List")
	FString selected;

	FEmergencePersonaListResponse() {};

	FEmergencePersonaListResponse( TArray<FEmergencePersona> _personas, FString _selected ){

		personas = _personas;
		selected = _selected;
	
	}
	
	FEmergencePersonaListResponse(FString _json_){
		FEmergencePersonaListResponse _tmpEmergencePersonaListResponse;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersonaListResponse>(
		_json_,
		&_tmpEmergencePersonaListResponse,
		0, 0);
		
		personas = _tmpEmergencePersonaListResponse.personas;
		selected = _tmpEmergencePersonaListResponse.selected;
	}
	
};