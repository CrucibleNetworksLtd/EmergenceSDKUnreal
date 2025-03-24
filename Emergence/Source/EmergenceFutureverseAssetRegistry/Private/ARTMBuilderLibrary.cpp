// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "ARTMBuilderLibrary.h"
#include "EmergenceCore.h"

FString UARTMBuilderLibrary::GenerateARTM(FString Message, TArray<FFutureverseARTMOperation> ARTMOperations, FString Address, FString Nonce)
{
    UE_LOG(LogEmergence, Display, TEXT("Generating ARTM with message %s, %d operations, for address %s, with nonce %s"), *Message, ARTMOperations.Num(), *Address, *Nonce);
    TMap< EFutureverseARTMOperationType, FString> OperationTypeStrings = {
        {EFutureverseARTMOperationType::CREATELINK, "asset-link create"},
        {EFutureverseARTMOperationType::DELETELINK, "asset-link delete"}
    };

    FString ARTM = "Asset Registry transaction\n\n";
    ARTM += Message + "\n\n";
    ARTM += "Operations:\n\n";
    for (FFutureverseARTMOperation Operation : ARTMOperations) {
        if (Operation.OperationType == EFutureverseARTMOperationType::NONE) {
            continue;
        }
        TArray<FString> Array;
        Operation.Slot.ParseIntoArray(Array, TEXT(":"), false);
        if((Array.Num() == 0) || Array.Last(0).IsEmpty() || Operation.LinkA.IsEmpty() || Operation.LinkB.IsEmpty()){
            UE_LOG(LogEmergence, Error, TEXT("Error parsing ARTM Operation!"));
            continue;
        }
        ARTM += *OperationTypeStrings.Find(Operation.OperationType) + "\n";
        ARTM += "- " + Array.Last(0) + "\n";
        ARTM += "- " + Operation.LinkA + "\n";
        ARTM += "- " + Operation.LinkB + "\n";
        ARTM += "end\n\n";
    }
    ARTM += "Operations END\n\n";
    ARTM += "Address: " + Address + "\n";
    ARTM += "Nonce: " + Nonce;
    return ARTM;
}
