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

	//used to create a new action object 
	static UActionObject* NewActionObject(UMissionObject* Outter, TSubclassOf<UActionObject> Class);
	//create object after loading the class
	static UActionObject* NewActionObject(UMissionObject* Outter, TSoftClassPtr<UActionObject> ClassPtr);

	static void SubscribeToMissionTick(UMissionObject* mission, UActionObject* ActivatedAction);

	static void InvokeActivateNext(int32 ObjectiveID, UMissionObject* OwningMission);

	static void InvokeMissionUpdate(UMissionObject* mission);

	/*root logics here are deprecated*/
	static void AddActionToRoot(UActionObject* action);
	static void RemoveActionFromRoot(UActionObject* action);
	/*root logics here are deprecated*/

	static void TutorialActionSelfPreform(TSubclassOf<UMissionObject> tutorialMission, TSubclassOf<UActionObject> tutorialAction);

};
