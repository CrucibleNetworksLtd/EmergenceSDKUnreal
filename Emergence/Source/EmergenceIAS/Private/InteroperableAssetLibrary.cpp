#include "InteroperableAssetLibrary.h"
#include "EmergenceInteroperableAssetElement.h"

UEmergenceInteroperableAssetElement* UEmergenceInteroperableAssetLibrary::GetInteroperableAssetElement(TArray<UEmergenceInteroperableAssetElement*> Array, TSubclassOf<UEmergenceInteroperableAssetElement> ElementClass, bool& success)
{
	for (auto Type : Array) {
		if (Type->GetClass() == ElementClass) {
			success = true;
			return Type;
		}
	}
	success = false;
	return nullptr;
}