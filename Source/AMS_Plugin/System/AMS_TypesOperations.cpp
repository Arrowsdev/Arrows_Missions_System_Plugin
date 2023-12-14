//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_TypesOperations.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include "AMS_Plugin/Public/ActionObject.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

AMS_TypesOperations::AMS_TypesOperations()
{
}

AMS_TypesOperations::~AMS_TypesOperations()
{
}

void AMS_TypesOperations::InvokeOnTaskActivated(UMissionObject* mission, TSoftClassPtr<UActionObject> ActivatedTask, int32 ActionCount, int32 totalCount)
{
	mission->OnTaskActivated(ActivatedTask.LoadSynchronous(), ActionCount, totalCount);
}

void AMS_TypesOperations::InvokeOnTaskFinished(UMissionObject* mission, TSoftClassPtr<UActionObject> ActivatedTask, int32 ActionCount)
{
	mission->OnTaskFinished(ActivatedTask.LoadSynchronous(), ActionCount);
}

//Deprecated
UActionObject* AMS_TypesOperations::NewActionObject(UMissionObject* Outter, TSubclassOf<UActionObject> Class)
{
	return NewObject<UActionObject>(Outter, Class);
}

UActionObject* AMS_TypesOperations::NewActionObject(UMissionObject* Outter, TSoftClassPtr<UActionObject> ClassPtr)
{
	return NewObject<UActionObject>(Outter, ClassPtr.LoadSynchronous());
}

void AMS_TypesOperations::SubscribeToMissionTick(UMissionObject* mission, UActionObject* ActivatedAction)
{
	mission->MissionTickDelegate.AddUObject(ActivatedAction, &UActionObject::ActionTick);
}

void AMS_TypesOperations::InvokeActivateNext(int32 ObjectiveID, UMissionObject* OwningMission)
{
	OwningMission->MissionDetails.ActivateNext(ObjectiveID, OwningMission);
}

void AMS_TypesOperations::InvokeMissionUpdate(UMissionObject* mission)
{
	mission->MissionHasUpdated();
}

void AMS_TypesOperations::AddActionToRoot(UActionObject* action)
{
	//action->AddToRoot();
}

void AMS_TypesOperations::RemoveActionFromRoot(UActionObject* action)
{
	//action->RemoveFromRoot();
}

void AMS_TypesOperations::TutorialActionSelfPreform(TSubclassOf<UMissionObject> tutorialMission, TSubclassOf<UActionObject> tutorialAction)
{
	if (MissionSubSystemInstance)
	{
		MissionSubSystemInstance->PreformTutorialAction(tutorialMission, tutorialAction);
	}
}

