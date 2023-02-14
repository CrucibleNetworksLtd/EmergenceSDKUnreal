// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "UnitConverterFunctionLibrary.h"

FString UUnitConverterFunctionLibrary::ConvertTest() 
{
	bool succeded = true;

	succeded &= Convert("2,3", EEtherUnitType::ETHER, EEtherUnitType::ETHER, ",") == "2,3";
	succeded &= Convert("0,0055", EEtherUnitType::GWEI, EEtherUnitType::GWEI, ",") == "0,0055";

	succeded &= Convert("2", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "2000000000";
	succeded &= Convert("2", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "2000000000000000000";

	succeded &= Convert("2", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "2000000000";
	succeded &= Convert("2", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,000000002";

	succeded &= Convert("200", EEtherUnitType::WEI, EEtherUnitType::GWEI, ",") == "0,0000002";
	succeded &= Convert("200", EEtherUnitType::WEI, EEtherUnitType::ETHER, ",") == "0,0000000000000002";

	succeded &= Convert("200", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "200000000000";
	succeded &= Convert("200", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,0000002";

	succeded &= Convert("2004", EEtherUnitType::WEI, EEtherUnitType::GWEI, ",") == "0,000002004";
	succeded &= Convert("2004", EEtherUnitType::WEI, EEtherUnitType::ETHER, ",") == "0,000000000000002004";

	succeded &= Convert("2004", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "2004000000000";
	succeded &= Convert("2004", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,000002004";

	succeded &= Convert("8,43092", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "8430920000";
	succeded &= Convert("8,43092", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "8430920000000000000";
	
	succeded &= Convert("532,3286", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "532328600000";
	succeded &= Convert("532,3286", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "532328600000000000000";

	succeded &= Convert("532,3286", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,0000005323286";
	succeded &= Convert("532,3286", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "532328600000";

	succeded &= Convert("0,843092", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "843092000";
	succeded &= Convert("0,843092", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "843092000000000000";
	
	succeded &= Convert("0,00843092", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "8430920";
	succeded &= Convert("0,00843092", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "8430920000000000";

	succeded &= Convert("0,0084309200000002", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "8430920,0000002";
	succeded &= Convert("0,0084309200000002", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "8430920000000200";
	
	succeded &= Convert("0,00843092", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,00000000000843092";
	succeded &= Convert("0,00843092", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "8430920";

	succeded &= Convert("12,0084309200000002", EEtherUnitType::ETHER, EEtherUnitType::GWEI, ",") == "12008430920,0000002";
	succeded &= Convert("12,0084309200000002", EEtherUnitType::ETHER, EEtherUnitType::WEI, ",") == "12008430920000000200";

	succeded &= Convert("12,0084309200000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,0000000120084309200000002";
	succeded &= Convert("12,0084309200000002", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "12008430920,0000002";

	succeded &= Convert("12,0084309200000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,0000000120084309200000002";
	succeded &= Convert("12,008430922", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "12008430922";

	succeded &= Convert("12008430,9200000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,0120084309200000002";
	succeded &= Convert("120084309,200000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,120084309200000002";
	succeded &= Convert("1200843092,00000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "1,20084309200000002";
	succeded &= Convert("12008430920,0000002", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "12,0084309200000002";

	succeded &= Convert("3,00843092", EEtherUnitType::GWEI, EEtherUnitType::WEI, ",") == "3008430920";
	succeded &= Convert("3,00843092", EEtherUnitType::GWEI, EEtherUnitType::ETHER, ",") == "0,00000000300843092";

	succeded &= Convert("3,00843092", EEtherUnitType::ETHER, EEtherUnitType::KWEI, ",") == "3008430920000000";
	succeded &= Convert("3,00843092", EEtherUnitType::ETHER, EEtherUnitType::TWEI, ",") == "3008430,92";
	succeded &= Convert("3,00843092", EEtherUnitType::ETHER, EEtherUnitType::METHER, ",") == "0,00000300843092";
	succeded &= Convert("3,00843092", EEtherUnitType::ETHER, EEtherUnitType::TETHER, ",") == "0,00000000000300843092";

	succeded &= Convert("1000034343434343434344", EEtherUnitType::WEI, EEtherUnitType::ETHER, ",") == "1000,034343434343434344";
	succeded &= Convert("1000034343434343434344", EEtherUnitType::WEI, EEtherUnitType::KETHER, ",") == "1,000034343434343434344";
	succeded &= Convert("100003434343434343434422", EEtherUnitType::WEI, EEtherUnitType::METHER, ",") == "0,100003434343434343434422";

	succeded &= Convert("53232860000000000000000000", EEtherUnitType::WEI, EEtherUnitType::ETHER, ",") == "53232860";
	succeded &= Convert("0", EEtherUnitType::WEI, EEtherUnitType::ETHER, ",") == "0";

	return succeded ? "True" : "False";
}


FString UUnitConverterFunctionLibrary::Convert(const FString& source, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, const FString& comaSeparator)
{
	if (sourceUnit == targetUnit)
	{
		return FString(source);
	}

	FString beforeComa;
	FString afterComa;
	if (source.Split(comaSeparator, &beforeComa, &afterComa))
	{
		return RemoveTrailingDecimals(Convert(source, beforeComa, afterComa, sourceUnit, targetUnit, comaSeparator), comaSeparator);
	}

	int8 resultLength = GetPowFactorDifference(sourceUnit, targetUnit);

	if (resultLength > 0)
	{
		FString result(source);

		for (int i = 0; i < resultLength; i++)
		{
			result += "0";
		}

		return result;
	}

	if (source.Len() > -resultLength)
	{
		FString result(source);
		result.InsertAt(source.Len() + resultLength, comaSeparator);
		return RemoveTrailingDecimals(result, comaSeparator);
	}

	FString result("0");
	result += comaSeparator;
	
	for (int i = resultLength + source.Len(); i < 0; i++)
	{
		result += "0";
	}

	result += TrimRightmostZeros(source);

	return RemoveTrailingDecimals(result, comaSeparator);
}

FString UUnitConverterFunctionLibrary::Convert(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, 
	                                           const FString& comaSeparator)
{
	int8 resultLength = GetPowFactorDifference(sourceUnit, targetUnit);
	FString result(beforeComa);

	if (resultLength > 0)
	{
		return ConvertToSmallerPow(source, beforeComa, afterComa, sourceUnit, targetUnit, comaSeparator, resultLength, result);
	}
	else
	{
		return ConvertToBiggerPow(source, beforeComa, afterComa, sourceUnit, targetUnit, comaSeparator, resultLength, result);
	}
}

FString UUnitConverterFunctionLibrary::ConvertToSmallerPow(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit,
	                                                       EEtherUnitType targetUnit, const FString& comaSeparator, int8 resultLength, FString& result)
{
	if (afterComa.Len() >= resultLength)
	{
		result += afterComa;

		if (result.Len() > resultLength + beforeComa.Len())
		{
			result.InsertAt(resultLength + beforeComa.Len(), comaSeparator);
		}
		result = TrimLeftmostZeros(result);

		return result;
	}
	else
	{
		result += afterComa;

		for (int i = 0; i < resultLength - afterComa.Len(); i++)
		{
			result += "0";
		}
		return TrimLeftmostZeros(result);
	}
}

FString UUnitConverterFunctionLibrary::ConvertToBiggerPow(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit, 
	                                                      EEtherUnitType targetUnit, const FString& comaSeparator, int8 resultLength, FString& result)
{
	if (beforeComa.Len() > -resultLength)
	{
		result += afterComa;

		if (result.Len() > resultLength + beforeComa.Len())
		{
			result.InsertAt(resultLength + beforeComa.Len(), comaSeparator);
		}
		return TrimLeftmostZeros(result);
	}
	else
	{
		result += afterComa;

		for (int i = resultLength + beforeComa.Len(); i < 0; i++)
		{
			result.InsertAt(0, "0");
		}

		result.InsertAt(0, comaSeparator);
		result.InsertAt(0, "0");

		return result;
	}
}

uint8 UUnitConverterFunctionLibrary::GetPowFactor(EEtherUnitType unit)
{
	static uint8 pow10[11] = { 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30 };
	return pow10[static_cast<uint8>(unit)];
}

int8 UUnitConverterFunctionLibrary::GetPowFactorDifference(EEtherUnitType sourceUnit, EEtherUnitType targetUnit)
{
	return GetPowFactor(sourceUnit) - GetPowFactor(targetUnit);
}

FString UUnitConverterFunctionLibrary::TrimRightmostZeros(const FString& source)
{
	uint8 trimIndex = source.Len();

	for (int i = source.Len() - 1; i >= 0; i--)
	{
		if (source[i] != '0')
		{
			trimIndex = i;
			break;
		}
	}

	return source.Mid(0, trimIndex + 1);
}

FString UUnitConverterFunctionLibrary::TrimLeftmostZeros(const FString& source)
{
	uint8 trimIndex = 0;

	for (int i = 0; i < source.Len(); i++)
	{
		if (source[i] != '0')
		{
			trimIndex = i;
			break;
		}
	}

	return source.Mid(trimIndex);
}

FString UUnitConverterFunctionLibrary::RemoveTrailingDecimals(const FString& source, const FString& comaSeparator)
{
	if (!source.Contains(comaSeparator))
	{
		return source;
	}

	uint8 trimIndex = source.Len();

	for (int i = source.Len() - 1; i >= 0; i--)
	{
		if (source[i] != '0')
		{
			trimIndex = i;
			if (source[i] == comaSeparator[0])
			{
				trimIndex--;
			}
			break;
		}
	}

	return source.Mid(0, trimIndex + 1);
}