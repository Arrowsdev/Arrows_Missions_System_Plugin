//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_SubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "KismetCompilerModule.h"
#include <Kismet2/KismetEditorUtilities.h>

#include "AMS_Plugin/Public/MissionObject.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"
#include "AMS_Plugin/Public/AMS_JuernalObject.h"
#include "AMS_Plugin/System/AMS_DataCenter.h"








UAMS_SubSystem::UAMS_SubSystem()
{
	LOG_AMS("Loaded", 10.0f, FColor::Green);

}

void UAMS_SubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	MissionSubSystemInstance = this;

	InitiateFullGameProgressData();
	UE_LOG(LogTemp, Warning, TEXT("Missions Count Is Calculated to be : %d"), FullGameMissionsCount);

	if (JuernalClass)
		JuernalSingelton = NewObject<UAMS_JuernalObject>(this, JuernalClass);
	else
		LOG_AMS("No Juernal", 10.0f, FColor::Red);

	if (DataCenterClass)
		DataCenterSinglton = NewObject<UAMS_DataCenter>(this, DataCenterClass);
	else
		LOG_AMS("No Data Center", 10.0f, FColor::Red);

	if (!SaveGameClass)
		LOG_AMS("No Save Game Class", 10.0f, FColor::Red);

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

	newRecord.RequiredCount = Mission->RequiredObjectivesCount;
	newRecord.BlackListedCount = Mission->BlackListedObjectivesCount;
	newRecord.MissionDetails.MissionProgress = Mission->GetMissionProgress();

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

		SaveGameObject->SG_FinishedMissions = GetFinishedMissions();
		SaveGameObject->SG_ActiveMissionsWhenSaved = GenerateRecordsFromActiveMissions();
		
		if (SaveGameObject->SG_ActiveMissionsWhenSaved.IsEmpty())
		{
			FString Mes = FString::Printf(TEXT("No Active Mission in profile [ %s ] saved but finished are : %d"),*ActiveSaveProfileName.ToString(), SaveGameObject->SG_FinishedMissions.Num());
			LOG_AMS(Mes, 10.0f, FColor::Yellow);
		}
		InvokeDataCenterSaveEvent(SaveGameObject);
		//UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
		FString Mes = FString::Printf(TEXT("Saved Game : Active = %d , Finished = %d "), SaveGameObject->SG_ActiveMissionsWhenSaved.Num(), SaveGameObject->SG_FinishedMissions.Num());
		LOG_AMS(Mes, 10.0f, FColor::Green);
	}

	else
	{
		UAMS_SaveGame* SaveGameObject = Cast<UAMS_SaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
		SaveGameObject->SG_FinishedMissions = FinishedMissions;
		SaveGameObject->SG_ActiveMissionsWhenSaved = GenerateRecordsFromActiveMissions();
		InvokeDataCenterSaveEvent(SaveGameObject);
		LOG_AMS("Save Game Initiated", 10.0f, FColor::Green);
		//UGameplayStatics::SaveGameToSlot(SaveGameObject, ActiveSaveProfileName.ToString(), 0);
	}
}


void UAMS_SubSystem::Internal_CompleteSave(UAMS_SaveGame* saveGameObject)
{
	LOG_AMS("Internal Save Was Called", 10.0f);
	UGameplayStatics::SaveGameToSlot(saveGameObject, ActiveSaveProfileName.ToString(), 0);
}

void UAMS_SubSystem::StartMission(TSubclassOf<UMissionObject> newMission)
{
	UMissionObject* StartedMission = NewObject<UMissionObject>(this, newMission);
	StartedMission->InitializeMission(INIT_START);

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
			LOG_AMS("Mission Activated", 10.0f, FColor::Magenta);
			ActivatedMission->MissionDetails = record.MissionDetails;
			ActivatedMission->InitializeMission(INIT_LOAD);
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

void UAMS_SubSystem::GenerateActiveMissionsFromRecord(UAMS_SaveGame* saveGameObject)
{
	TArray<FRecordEntry> ActiveRecords = saveGameObject->SG_ActiveMissionsWhenSaved;

	ActiveMissions.Empty();
	if (ActiveRecords.IsEmpty())
	{
		LOG_AMS("No Active Records", 10.0f, FColor::Red);
	}
	for (auto& record : ActiveRecords)
	{
		if (!record.MissionClass)
		{
			LOG_AMS("the record has invalid mission class", 10.0f, FColor::Red);
		}
		UMissionObject* ActivatedMission = NewObject<UMissionObject>(this, record.MissionClass);
		if (ActivatedMission)
		{
			LOG_AMS("Mission Activated", 10.0f, FColor::Magenta);
			ActivatedMission->MissionDetails = record.MissionDetails;
			ActivatedMission->RequiredObjectivesCount = record.RequiredCount;
			ActivatedMission->BlackListedObjectivesCount = record.BlackListedCount;

			ActivatedMission->InitializeMission(INIT_LOAD);
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
	for (auto& kvPair : ActiveMissions)
	{
		int32 required = kvPair.Value->RequiredObjectivesCount;
		int32 blacklisted = kvPair.Value->BlackListedObjectivesCount;

		Records.Emplace(kvPair.Key, kvPair.Value->MissionDetails, required, blacklisted);
	}
	return Records;
}

TArray<FRecordEntry>& UAMS_SubSystem::GetFinishedMissions()
{
	if (JuernalSingelton)
	{
		return JuernalSingelton->GetMissionsRecord();
	}

	return FinishedMissions;//used the one saved in the sybsystem , i guess ama keep it around for this porpose if the user have noe juernal

}

void UAMS_SubSystem::LoadFinishedMissionsToJuernal()
{
	if (JuernalSingelton)
	{
		return JuernalSingelton->AssignRecordList(FinishedMissions);
	}
}

UAMS_SubSystem* UAMS_SubSystem::GetMissionSubSystem()
{
	return MissionSubSystemInstance;
}

void UAMS_SubSystem::InvokeDataCenterSaveEvent(UAMS_SaveGame* saveGameObject)
{
	if (DataCenterSinglton)
	{
		DataCenterSinglton->OnGameSaveStarted(saveGameObject);
	}
}

void UAMS_SubSystem::InvokeDataCenterLoadEvent(UAMS_SaveGame* saveGameObject)
{
	if (DataCenterSinglton)
	{
		DataCenterSinglton->OnGameLoaded(saveGameObject);
	}
}

void UAMS_SubSystem::PreformMissionAction(TSubclassOf<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction)
{
	UActionObject* preformedActionCDO = PreformedAction.GetDefaultObject();

	if (ActiveMissions.Contains(Mission))
	{
		FObjective& objective = ActiveMissions[Mission]->MissionDetails.GetActionRelatedObjective(PreformedAction);
		if (objective.ActionClass && !objective.bIsFinished)
		{
			LOG_AMS("Subsystem preform called", 10.0f, FColor::Yellow);
			if (objective.Preform())//this means this preform was the finisher for it so we check if all tasks are finished too
			{
				ActiveMissions[Mission]->MissionCheckEnd(PreformedAction);//either passed or failed

				if (SaveType == ESaveMissionType::Always)
				{
					Internal_MissionSave();
					LOG_AMS("Saved After Objective Finished", 10.0f, FColor::Magenta);
				}
					
			}

			else//the action is preformed but the objective is not finished we check if it was a blacklisted action with instant fail flag
			{

				if (preformedActionCDO->ActionType == EActionType::blacklisted && preformedActionCDO->bInstantFail)
				{
					ActiveMissions[Mission]->EndMission(EFinishState::failed, FFailInfo(PreformedAction));
				}
			}
		}

		else
		  LOG_AMS("Objective was not found, the loading comes with missig data", 10.0f, FColor::Red);
		
	}
}

UAMS_SaveGame* UAMS_SubSystem::CreateMissionSaveObject()
{
	return nullptr;//probably dont need it if all saving is going to be done with the data center , for now i'll leave it if i 
	//wanted to add manual saving instead of hooking data to autosaving
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
		GenerateActiveMissionsFromRecord(SaveGameObject);

		FinishedMissions = SaveGameObject->SG_FinishedMissions;

		LoadFinishedMissionsToJuernal();

		InvokeDataCenterLoadEvent(SaveGameObject);

		return SaveGameObject;
	}
	else
	{
		FString Mes = FString::Printf(TEXT("the profile [ %s ] was not found"), *playerProfile.ToString());
		LOG_AMS(Mes, 10.0f, FColor::Red);
		return nullptr;
	}

}

void UAMS_SubSystem::CreateCheckPoint()
{
	CheckPointMissionsRecords = GenerateRecordsFromActiveMissions();
}

void UAMS_SubSystem::LoadCheckPoint()
{
	//shoul put retrevie logics here
	//we have an issue , if the system was used for rpg game and the player has multiple opened quests and one is failed 
	//when restarting the game from checkpoint we need a way to figure out the place that should be uesed for respawn , is it for the first mission ?
	//or the second or what mission, or maybe i dont know much about rpg missions and how they are played and if even they have the ability to start multiple quests
	//i need some directions in this matter 
}

void UAMS_SubSystem::CancelMission(TSubclassOf<UMissionObject> mission)
{
	if (ActiveMissions.Contains(mission))
	{
		ActiveMissions[mission]->EndMission(EFinishState::canceled, FFailInfo(EMissionFailReason::canceled));
	}
}

void UAMS_SubSystem::Internal_GetGameProgress()
{
	
}

//called once on begin play so that the game knows the count of games missions 
void UAMS_SubSystem::InitiateFullGameProgressData()
{
	TArray<FAssetData> FoundMissions;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FTopLevelAssetPath BaseClassName = UMissionObject::StaticClass()->GetDefaultObject()->GetClass()->GetClassPathName();

	//the tag that is added on mission construct
	TArray<FName>Tags;
	Tags.Add("MissionObject");
	AssetRegistryModule.Get().GetAssetsByTags(Tags, FoundMissions);

	FullGameMissionsCount = FoundMissions.Num();

	if (FullGameMissionsCount)
	{
		FullGameMissionsRecords.Empty();
		for (FAssetData& mission : FoundMissions)
		{

			UBlueprint* MissionBlueprint = Cast<UBlueprint>(mission.GetAsset());
			if (MissionBlueprint)
			{
				UMissionObject* missionObject = Cast<UMissionObject>(MissionBlueprint->GeneratedClass.GetDefaultObject());
				if (missionObject)
				{
					FMissionDetails recordDetails = AMS_Types::GenerateDetails(missionObject->MissionRelatedActions);
					FRecordEntry newRecord = FRecordEntry(missionObject->GetClass(), recordDetails);
					FullGameMissionsRecords.Add(newRecord);
				}
			}

			
		}
	}
    
	
	PrintLog(FString::Printf(TEXT("found %d of Mission Object Blueprints and first one is :  "), FoundMissions.Num()), 10.0f);
}
