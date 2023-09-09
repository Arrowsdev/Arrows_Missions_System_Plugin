//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_TypesOperations.h"
#include "AMS_Plugin/Public/MissionObject.h"

AMS_TypesOperations::AMS_TypesOperations()
{
}

AMS_TypesOperations::~AMS_TypesOperations()
{
}

void AMS_TypesOperations::InvokeOnTaskActivated(UMissionObject* mission, TSubclassOf<UActionObject> ActivatedTask, int32 ActionCount, int32 totalCount)
{
	mission->OnTaskActivated(ActivatedTask, ActionCount, totalCount);
}

void AMS_TypesOperations::InvokeOnTaskFinished(UMissionObject* mission, TSubclassOf<UActionObject> ActivatedTask, int32 ActionCount)
{
	mission->OnTaskFinished(ActivatedTask, ActionCount);
}