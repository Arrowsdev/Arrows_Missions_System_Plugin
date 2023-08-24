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
	//Internal_MissionSave();//this makes an empty save profile for this new gameplay so we can store progress directly
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
//(fixed from the properties specifires need to make them configs)
void UAMS_SubSystem::Internal_MissionSave()
{
    if(UGameplayStatics::DoesSaveGameExist(ActiveSaveProfileName.ToString(), 0))
	{
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::LoadGameFromSlot(ActiveSaveProfileName.ToString(), 0));

		SaveGameObject->SG_FinishedMissions = FinishedMissions;
		SaveGameObject->SG_ActiveMissionsWhenSaved = GenerateRecordsFromActiveMissions();

		if (SaveGameObject->SG_ActiveMissionsWhenSaved.IsEmpty())
		{
			FString Mes = FString::Printf(TEXT("No Active Mission in profile [ %s ] saved but finished are : %d"),*ActiveSaveProfileName.ToString(), SaveGameObject->SG_FinishedMissions.Num());
			LOG_AMS(Mes, 10.0f, FColor::Yellow);
		}
		//UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
		FString Mes = FString::Printf(TEXT("Saved Game : Active = %d , Finished = %d "), SaveGameObject->SG_ActiveMissionsWhenSaved.Num(), SaveGameObject->SG_FinishedMissions.Num());
		LOG_AMS(Mes, 10.0f, FColor::Green);
		BroadcastSubsystemSave(SaveGameObject, SAVE_PROCESS);//should replace the upper line 
	}

	else
	{
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
		//UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
		BroadcastSubsystemSave(SaveGameObject, SAVE_PROCESS);
	}
}

void UAMS_SubSystem::BroadcastSubsystemSave(UAMS_SaveGame* saveGameObject, bool bIsSave)
{
	if (bIsSave) goto broadcast_Save;

	for (auto mission : ActiveMissions)
	{
		mission.Value->OnGameLoaded(saveGameObject);
	}

	return;

	broadcast_Save:
	for (auto mission : ActiveMissions)
	{
		mission.Value->OnGameSaveStarted(saveGameObject);
	}
}

void UAMS_SubSystem::Internal_CompleteSave(UAMS_SaveGame* saveGameObject)
{
	UGameplayStatics::SaveGameToSlot(saveGameObject, ActiveSaveProfileName.ToString(), 0);
	OnMissoinSystemSave.Broadcast(saveGameObject);
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
	if (ActiveRecords.IsEmpty())
	{
		LOG_AMS("No Active Records", 10.0f, FColor::Red);
	}
	for (auto record : ActiveRecords)
	{
		if (!record.MissionClass)
		{
			LOG_AMS("the record has invalid mission class", 10.0f, FColor::Red);
		}
		UMissionObject* ActivatedMission = NewObject<UMissionObject>(this, record.MissionClass);
		if (ActivatedMission)
		{
			LOG_AMS("Mission Activated", 10.0f, FColor::Green);
			ActivatedMission->MissionDetails = record.MissionDetails;
			ActivatedMission->InitializeMission();
			ActiveMissions.Add(record.MissionClass, ActivatedMission);
		}
		
	}

	if (!ActiveMissions.IsEmpty())
	{
		LOG_AMS("Active missions list generated from records", 10.0f, FColor::Green);
	}

	else
	LOG_AMS("Failed To Generate Acive List", 10.0f, FColor::Red);
}

TArray<FRecordEntry> UAMS_SubSystem::GenerateRecordsFromActiveMissions()
{
	TArray<FRecordEntry> Records;
	for (auto kvPair : ActiveMissions)
	{
		Records.Emplace(kvPair.Key, kvPair.Value->MissionDetails);
	}
	return Records;
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

				if (SaveType == ESaveMissionType::Always)
					Internal_MissionSave();
			}

			else//the action is preformed but the objective is not finished we check if it was a blacklisted action with instant fail flag
			{
				

				if (preformedActionCDO->ActionType == EActionType::blacklisted && preformedActionCDO->bInstantFail)
				{
					ActiveMissions[Mission]->EndMission(EFinishState::failed, FFailInfo(PreformedAction));
				}
			}
		}
	}
}

UAMS_SaveGame* UAMS_SubSystem::CreateMissionSaveObject()
{
	return nullptr;
}

UAMS_SaveGame* UAMS_SubSystem::LoadGame(FName playerProfile, bool& found)
{
	if (UGameplayStatics::DoesSaveGameExist(playerProfile.ToString(), 0))
	{
		FString Mes = FString::Printf(TEXT("mission is loading , profile is : %s"), *playerProfile.ToString());
		LOG_AMS(Mes, 10.0f, FColor::Green);
		found = true;
		ActiveSaveProfileName = playerProfile;
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::LoadGameFromSlot(ActiveSaveProfileName.ToString(), 0));

		if (SaveGameObject->SG_ActiveMissionsWhenSaved.IsEmpty())
		{
			LOG_AMS("Saved Active Mission is Empty", 10.0f, FColor::Red);
		}
		GenerateActiveMissionsFromRecord(SaveGameObject->SG_ActiveMissionsWhenSaved);
		FinishedMissions = SaveGameObject->SG_FinishedMissions;
		BroadcastSubsystemSave(SaveGameObject, LOAD_PROCESS);
		return SaveGameObject;
	}
	else
	{
		FString Mes = FString::Printf(TEXT("the profile [ %s ] was not found"), *playerProfile.ToString());
		LOG_AMS(Mes, 10.0f, FColor::Red);
		return nullptr;
	}

}
