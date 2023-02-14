// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceTransaction
{

  GENERATED_BODY()

  //Address of the receiver. Null when its a contract creation transaction.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString To;

  //The transaction type.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int Type;

  //Array of log objects, which this transaction generated.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString Logs;

  //either 1 (success) or 0 (failure).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  bool Status;

  //If this transaction was to create a contract address, then this will be the address of the newly create contract, otherwise will be null.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString ContractAddress;

  //The actual value per gas deducted from the senders account. Before EIP-1559, this is equal to the transaction's gas price. After, it is equal to baseFeePerGas + min(maxFeePerGas - baseFeePerGas, maxPriorityFeePerGas). Legacy transactions and EIP-2930 transactions are coerced into the EIP-1559 format by setting both maxFeePerGas and maxPriorityFeePerGas as the transaction's gas price.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString EffectiveGasPrice;

  //The amount of gas used by this specific transaction alone.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString GasUsed;

  //The total amount of gas used when this transaction was executed in the block.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString CumulativeGasUsed;

  //Post-transaction stateroot.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString Root;

  //Address of the sender.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString From;

  //Block number where this transaction was in.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString BlockNumber;

  //Hash of the block where this transaction was in.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString BlockHash;

  //Integer of the transactions index position in the block.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString TransactionIndex;

  //Hash of the transaction.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString TransactionHash;

  //Bloom filter for light clients to quickly retrieve related logs.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString LogsBloom;

  //The amount of confirmations (chain's current block number - this transaction's block number).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int Confirmations;

  FEmergenceTransaction() {};

  FEmergenceTransaction( FString _To, int _Type, FString _Logs, bool _Status, FString _ContractAddress, FString _EffectiveGasPrice, FString _GasUsed, FString _CumulativeGasUsed, FString _Root, FString _From, FString _BlockNumber, FString _BlockHash, FString _TransactionIndex, FString _TransactionHash, FString _LogsBloom, int _Confirmations){

    To = _To;
    Type = _Type;
    Logs = _Logs;
    Status = _Status;
    ContractAddress = _ContractAddress;
    EffectiveGasPrice = _EffectiveGasPrice;
    GasUsed = _GasUsed;
    CumulativeGasUsed = _CumulativeGasUsed;
    Root = _Root;
    From = _From;
    BlockNumber = _BlockNumber;
    BlockHash = _BlockHash;
    TransactionIndex = _TransactionIndex;
    TransactionHash = _TransactionHash;
    LogsBloom = _LogsBloom;
    Confirmations = _Confirmations;
  }
  
  FEmergenceTransaction(FString _json_){
    FEmergenceTransaction _tmpEmergenceTransaction;
    
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceTransaction>(
		_json_,
		&_tmpEmergenceTransaction,
    0, 0);
    
    To = _tmpEmergenceTransaction.To;
    Type = _tmpEmergenceTransaction.Type;
    Logs = _tmpEmergenceTransaction.Logs;
    Status = _tmpEmergenceTransaction.Status;
    ContractAddress = _tmpEmergenceTransaction.ContractAddress;
    EffectiveGasPrice = _tmpEmergenceTransaction.EffectiveGasPrice;
    GasUsed = _tmpEmergenceTransaction.GasUsed;
    CumulativeGasUsed = _tmpEmergenceTransaction.CumulativeGasUsed;
    Root = _tmpEmergenceTransaction.Root;
    From = _tmpEmergenceTransaction.From;
    BlockNumber = _tmpEmergenceTransaction.BlockNumber;
    BlockHash = _tmpEmergenceTransaction.BlockHash;
    TransactionIndex = _tmpEmergenceTransaction.TransactionIndex;
    TransactionHash = _tmpEmergenceTransaction.TransactionHash;
    LogsBloom = _tmpEmergenceTransaction.LogsBloom;
    Confirmations = _tmpEmergenceTransaction.Confirmations;
  }
  
};