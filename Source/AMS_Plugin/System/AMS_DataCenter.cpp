//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_DataCenter.h"


UWorld* UAMS_DataCenter::GetWorld() const
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
