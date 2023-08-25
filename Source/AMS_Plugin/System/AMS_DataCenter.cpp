//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_DataCenter.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

void UAMS_DataCenter::OnGameSaveStarted_Implementation(UAMS_SaveGame* SaveGameObject)
{
	if (SaveGameObject)
	{
		CompleteSaving(SaveGameObject);
	}
	else
	{
		LOG_AMS("Save Object Is Not Valid", 10.0f, FColor::Red);
	}
}

void UAMS_DataCenter::CompleteSaving(UPARAM(ref)UAMS_SaveGame* saveGameObject)
{
	if (MissionSubSystemInstance)
	{
		MissionSubSystemInstance->Internal_CompleteSave(saveGameObject);
	}
	else
	LOG_AMS("Subsystem Singletone is not valid", 10.0f, FColor::Red);
}
