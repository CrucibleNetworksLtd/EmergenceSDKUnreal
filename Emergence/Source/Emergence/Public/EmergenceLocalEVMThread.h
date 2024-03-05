// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "Core.h"
#include "HAL/Runnable.h"
#include "WalletService/WriteMethod.h"

class UWriteMethod;

struct EMERGENCE_API EmergenceLocalEVMJSON
{
	int a;
	const char* b;
	const char* _address;
	const char* _ABI;
	const char* _network;
	const char* _nodeUrl;
	const char* _methodName;
	const char* _localAccountName;
	const char* _password;
	const char* _keystorePath;
	const char* _chainID;
	const char* _gasPrice;
	const char* _value;
	const char* _privateKey;
	const char* _publicKey;
	volatile char* result; // to store the result
	volatile int ResultLength;
};

class EMERGENCE_API FLocalEVMThreadRunnable : public FRunnable
{
public:

	FLocalEVMThreadRunnable() {};

	//override Init,Run and Stop.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	UWriteMethod* WriteMethod;
	wchar_t fullpath[2048];
	int length;
	EmergenceLocalEVMJSON* Data;
	typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length, EmergenceLocalEVMJSON* Data);
	_getExampleLibraryFunction ExampleLibraryFunction;
};