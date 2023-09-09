//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"

class UMissionObject;
class UActionObject;

/**
 * this class is meant to work around the dependacy between the types and the system classes 
 */
class AMS_PLUGIN_API AMS_TypesOperations
{
public:
	AMS_TypesOperations();
	~AMS_TypesOperations();

	//used to call the on task activated on the owning mission
	static void InvokeOnTaskActivated(UMissionObject* mission, TSubclassOf<UActionObject> ActivatedTask, int32 ActionCount, int32 totalCount);

	//used to call the on task finished on the owning mission
	static void InvokeOnTaskFinished(UMissionObject* mission, TSubclassOf<UActionObject> ActivatedTask, int32 ActionCount);
};
