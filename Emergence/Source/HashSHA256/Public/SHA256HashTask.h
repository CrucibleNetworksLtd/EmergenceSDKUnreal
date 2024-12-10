// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "HAL/ThreadSafeCounter.h"
#include "Containers/Queue.h"

/*
	Example usage:

	TArray<FString> MyFiles;
	MyFiles.Add("a.txt");
	MyFiles.Add("b.txt");
	MyFiles.Add("c.txt");

	FThreadSafeCounter BuildTasksCounter;
	FSHA256FileHasherTask::FResultQueue Results;

	for (const FString& File : MyFiles)
	{
		// The tasks will clean themselves up when they are completed; no need to call delete elsewhere.

		(new FAutoDeleteAsyncTask<FSHA256FileHasherTask>(
			&BuildTasksCounter,
			File,
			&Results))->StartBackgroundTask();
	}

	while (BuildTasksCounter.GetValue() > 0)
	{
		FPlatformProcess::Sleep(.1f);
	}

	while (!Results.IsEmpty())
	{
		FSHA256FileHasherTask::FHashResult Result;
		Results.Dequeue(Result);

		UE_LOG(LogTemp, Log, TEXT("%s [%s]"), *Result.Key, *Result.Value);
	}
*/

/*
* Async task for hashing a file in a thread pool.
* If the file does not exist an empty hash will be added to the result queue.
*/
class HASHSHA256_API FSHA256FileHasherTask : public FNonAbandonableTask
{
public:
	// Key: Filename, Value: Hash
	typedef TTuple<FString, FString> FHashResult;

	// TQueue is thread safe by default
	// Multiple producers: thread pool FSHA256FileHasherTask, Single consumer: Invoker
	typedef TQueue<FHashResult, EQueueMode::Mpsc> FResultQueue;

	/**
	 * Initialize the task as follows:
	 *
	 * @param InThreadScaleCounter  Used by the invoker to track when tasks are completed.
	 * @param InFullPathOnDisk      Filename to hash.
	 * @param OutResultQueue		Queue hashes are added to.
	 */
	FSHA256FileHasherTask(
		FThreadSafeCounter* InThreadScaleCounter,
		const FString& InFullPathOnDisk,
		FResultQueue* OutResultQueue)
		: ThreadScaleCounter(InThreadScaleCounter)
		, FullPathOnDisk(InFullPathOnDisk)
		, ResultQueue(OutResultQueue)
	{
		checkSlow(ThreadScaleCounter && ResultQueue);

		ThreadScaleCounter->Increment();
	}

	/** Called by the thread pool to do the work in this task */
	void DoWork(void);

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSHA256FileHasherTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FThreadSafeCounter* ThreadScaleCounter;
	const FString FullPathOnDisk;
	FResultQueue* ResultQueue;
};
