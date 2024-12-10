// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#include "SHA256HashTask.h"
#include "SHA256Hash.h"
#include "Runtime/Launch/Resources/Version.h"

void FSHA256FileHasherTask::DoWork(void)
{
	// Pool thread

	FSHA256Hash Hash;
	Hash.FromFile(FullPathOnDisk);

#if ENGINE_MAJOR_VERSION == 5
	ResultQueue->Enqueue({ FullPathOnDisk, Hash.GetHash() });
#else
	ResultQueue->Enqueue(TPair<FString, FString>(FullPathOnDisk, Hash.GetHash()));
#endif

	ThreadScaleCounter->Decrement();
}
