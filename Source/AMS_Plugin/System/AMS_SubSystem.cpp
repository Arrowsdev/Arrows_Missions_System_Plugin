//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_SubSystem.h"
#include "Kismet/GameplayStatics.h"

#include "AMS_Plugin/Public/MissionObject.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"
#include "AMS_Plugin/Public/AMS_JuernalObject.h"

UAMS_SubSystem::UAMS_SubSystem()
{
	MissionSubSystemInstance = this;

	if(JuernalClass)
	JuernalSingelton = NewObject<UAMS_JuernalObject>(this, JuernalClass);
	//here should load any saved data and populate the nessassery data slots 

	LOG_AMS("Loaded", 10.0f, FColor::Green);
}

void UAMS_SubSystem::StartMission(TSubclassOf<UMissionObject> newMission, FName SaveProfileName)
{
	ActiveSaveProfileName = SaveProfileName;
	StartMission(newMission);
	Internal_MissionSave();//this makes an empty save profile for this new gameplay so we can store progress directly
}

void UAMS_SubSystem::RecordMissionFinished(UMissionObject* Mission)
{
	//record mission details and add the details to the finished array 
	FRecordEntry newRecord(Mission->GetClass(), Mission->MissionDetails);
	FinishedMissions.Add(newRecord);//deprecated , left for near futur only

	if(JuernalSingelton)
	JuernalSingelton->AddJuernalRecord(newRecord);

	if (ActiveMissions.Contains(Mission->GetClass()))
	{
		ActiveMissions.Remove(Mission->GetClass());
	}

	if (SaveType == ESaveMissionType::Post)
	Internal_MissionSave();
}

//bug : the save class is null , due to project settings are not saving or changing the subsystem cdo values 
void UAMS_SubSystem::Internal_MissionSave()
{
    if(UGameplayStatics::DoesSaveGameExist(ActiveSaveProfileName.ToString(), 0))
	{
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::LoadGameFromSlot(ActiveSaveProfileName.ToString(), 0));

		SaveGameObject->SG_FinishedMissions = FinishedMissions;
		SaveGameObject->SG_ActiveMissionsWhenSaved = GenerateRecordsFromActiveMissions();

		UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
	}

	else
	{
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
		UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
	}
}

void UAMS_SubSystem::StartMission(TSubclassOf<UMissionObject> newMission)
{
	UMissionObject* StartedMission = NewObject<UMissionObject>(this, newMission);
	StartedMission->InitializeMission();

	ActiveMissions.Add(newMission,StartedMission);
}


void UAMS_SubSystem::GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords)
{
	ActiveMissions.Empty();

	for (auto record : ActiveRecords)
	{
		UMissionObject* ActivatedMission = NewObject<UMissionObject>(this, record.MissionClass);
		ActivatedMission->MissionDetails = record.MissionDetails;

		ActiveMissions.Add(record.MissionClass,ActivatedMission);
	}
}

TArray<FRecordEntry> UAMS_SubSystem::GenerateRecordsFromActiveMissions()
{
	return TArray<FRecordEntry>();
}

UAMS_SubSystem* UAMS_SubSystem::GetMissionSubSystem()
{
	return MissionSubSystemInstance;
}

void UAMS_SubSystem::PreformMissionAction(TSubclassOf<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction)
{
	UActionObject* preformedActionCDO = PreformedAction.GetDefaultObject();

	if (ActiveMissions.Contains(Mission))
	{
		FObjective& objective = ActiveMissions[Mission]->MissionDetails.GetActionRelatedObjective(PreformedAction);
	
		if (objective.ActionClass)
		{
			if (objective.Preform())//this means this preform was the finisher for it so we check if all tasks are finished too
			{
				ActiveMissions[Mission]->MissionCheckEnd(PreformedAction);//either passed or failed
			}
		}
	}
}

UAMS_SaveGame* UAMS_SubSystem::CreateMissionSaveObject()
{
	return nullptr;
}

void UAMS_SubSystem::LoadGame(bool& found, FName playerProfile)
{
	if (UGameplayStatics::DoesSaveGameExist(playerProfile.ToString(), 0))
	{
		found = true;
		ActiveSaveProfileName = playerProfile;
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::LoadGameFromSlot(ActiveSaveProfileName.ToString(), 0));

		GenerateActiveMissionsFromRecord(SaveGameObject->SG_ActiveMissionsWhenSaved);
		FinishedMissions = SaveGameObject->SG_FinishedMissions;
	}

}
