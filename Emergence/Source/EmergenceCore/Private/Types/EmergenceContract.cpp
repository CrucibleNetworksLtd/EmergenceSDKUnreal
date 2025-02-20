// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "Types/EmergenceContract.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UEmergenceContract::UEmergenceContract(FString _ABI)
{
	this->ABI = _ABI;
	FindMethods();
}

UEmergenceContract* UEmergenceContract::CreateEmergenceContract(FString _ABI)
{
	UEmergenceContract* EmergenceContract = NewObject<UEmergenceContract>(UEmergenceContract::StaticClass());
	EmergenceContract->ABI = _ABI;
	EmergenceContract->FindMethods();
	return EmergenceContract;
}

void UEmergenceContract::FindMethods()
{
	this->ReadMethods.Empty();
	this->WriteMethods.Empty();
	if (!this->ABI.IsEmpty()) {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(this->ABI);
		TSharedPtr<FJsonValue> JsonObject;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
			auto ABIArray = JsonObject->AsArray();
			for (int i = 0; i < ABIArray.Num(); i++) {
				TSharedPtr<FJsonObject> InterfaceObject = ABIArray[i].Get()->AsObject();
				if (InterfaceObject->GetStringField("type") == "function") {
					if (InterfaceObject->GetStringField("stateMutability") == "view" || InterfaceObject->GetStringField("stateMutability") == "pure") {
						this->ReadMethods.Add(FEmergenceContractMethod(InterfaceObject->GetStringField("name")));
					}
					else {
						this->WriteMethods.Add(FEmergenceContractMethod(InterfaceObject->GetStringField("name")));
					}
				}
			}
		}
	}
}
