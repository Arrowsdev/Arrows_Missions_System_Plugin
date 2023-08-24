//Advanced Mission System Developed by AIS Studios @2023


#include "ActionObject.h"

UWorld* UActionObject::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}
