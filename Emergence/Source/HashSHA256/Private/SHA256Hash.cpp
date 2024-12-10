// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

#include "SHA256Hash.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Containers/ArrayView.h"
#include <picosha2.h>

#include <vector>

FSHA256Hash::FSHA256Hash()
{

}

FString FSHA256Hash::HashStringHelper(const FString& Str)
{
	FSHA256Hash SHA256;
	SHA256.FromString(Str);
	return SHA256.GetHash();
}

void FSHA256Hash::FromString(const FString& Str)
{
	std::string HashStr;
	picosha2::hash256_hex_string(std::string(TCHAR_TO_UTF8(*Str)), HashStr);
	Hash = UTF8_TO_TCHAR(HashStr.c_str());
}

void FSHA256Hash::FromArray(const TArray<uint8>& Arr)
{
	const auto ArrView = MakeArrayView(Arr.GetData(), Arr.Num());
	Hash = UTF8_TO_TCHAR(picosha2::hash256_hex_string(ArrView.begin(), ArrView.end()).c_str());
}

void FSHA256Hash::FromArray64(const TArray64<uint8>& Arr)
{
	const auto ArrView = TArrayView64<const uint8>(Arr.GetData(), Arr.Num());
	Hash = UTF8_TO_TCHAR(picosha2::hash256_hex_string(ArrView.begin(), ArrView.end()).c_str());
}

bool FSHA256Hash::FromFile(const FString& File)
{
	if (IFileHandle* FilePtr = IPlatformFile::GetPlatformPhysical().OpenRead(*File))
	{
		picosha2::hash256_one_by_one hasher;

		// read in 64K chunks to prevent raising the memory high water mark too much
		{
			static const int64 FILE_BUFFER_SIZE = 64 * 1024;
			std::vector<uint8> VecBuff(FILE_BUFFER_SIZE, 0);
			int64 FileSize = FilePtr->Size();
			for (int64 Pointer = 0; Pointer < FileSize;)
			{
				// how many bytes to read in this iteration
				int64 SizeToRead = FileSize - Pointer;
				if (SizeToRead > FILE_BUFFER_SIZE)
				{
					SizeToRead = FILE_BUFFER_SIZE;
				}
				// read dem bytes
				if (!FilePtr->Read(VecBuff.data(), SizeToRead))
				{
					UE_LOG(LogTemp, Error, TEXT("Read error while validating '%s' at offset %lld."), *File, Pointer);

					// don't forget to close
					delete FilePtr;
					return false;
				}
				Pointer += SizeToRead;

				// update the hash
				hasher.process(VecBuff.begin(), VecBuff.begin() + SizeToRead);
			}

			// done with the file
			delete FilePtr;
		}

		hasher.finish();

		Hash = UTF8_TO_TCHAR(picosha2::get_hash_hex_string(hasher).c_str());

		return true;
	}

	return false;
}

FString FSHA256Hash::GetHash() const
{
	return Hash;
}
