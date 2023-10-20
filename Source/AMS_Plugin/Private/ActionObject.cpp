//Advanced Mission System Developed by AIS Studios @2023


#include "ActionObject.h"

UWorld* UActionObject::GetWorld() const
{
#if WITH_EDITOR
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

#endif
	const UObject* Outer = GetOuter();
	if (Outer == nullptr)
	{
		return nullptr;
	}
	return Outer->GetWorld();
}
