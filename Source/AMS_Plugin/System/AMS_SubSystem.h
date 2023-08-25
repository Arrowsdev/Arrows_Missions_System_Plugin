//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMS_Types.h"
#include "AMS_LOG.h"
#include "AMS_SubSystem.generated.h"

/**
 * 
 */
class UAMS_JuernalObject;
class UAMS_SaveGame;
class UAMS_DataCenter;

static UAMS_SubSystem* MissionSubSystemInstance;

UCLASS(config=Engine , defaultconfig)
class AMS_PLUGIN_API UAMS_SubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UAMS_SubSystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

//PROJECT SETTINGS

    //the mission class that will be the first one \n if null you will have to manually call start mission when ever you want to start one
	UPROPERTY(config,EditAnywhere, Category = "Settings")
		TSubclassOf<UMissionObject> StartupMission;

	//the juernal class used to record progress and achivements
	UPROPERTY(config,EditAnywhere, Category = Settings)
		TSubclassOf<UAMS_JuernalObject> JuernalClass;

	//how the system handles saving progress
	UPROPERTY(config,EditAnywhere, Category = "Save Settings")
		ESaveMissionType SaveType;

	//the class used by the system to save missions progress , you should inheret from this class to make custom save file
	//depending on your game needs
	UPROPERTY(config,EditAnywhere, Category = "Save Settings", meta=(EditCondition="SaveType != ESaveMissionType::none"))
		TSubclassOf<UAMS_SaveGame> SaveGameClass;

	//data center class that you used to make your save game logics , there you hook the custom data with the saved mission data
	UPROPERTY(config, EditAnywhere, Category = "Save Settings", meta = (EditCondition = "SaveType != ESaveMissionType::none"))
		TSubclassOf<UAMS_DataCenter> DataCenterClass;

// END OF PROJECT SETTINGS

	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
    void StartMission(TSubclassOf<UMissionObject> newMission, FName SaveProfileName);

	//preform an action for specific mission from the active missions , if the provided mission is not active
	//the call will be ignored
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	void PreformMissionAction(TSubclassOf<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction);
	
	//this function creates a savegame object , if there was an old profile it loads the last progress and let you 
	//hook your new data too before saving the file again
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	UAMS_SaveGame* CreateMissionSaveObject();

	/*this function loads specific player profile data, returns a reference to the loaded object
	* so you can load the custom data you have saved
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		UAMS_SaveGame* LoadGame(FName playerProfile, bool& found);

	//get the records for the finished missions , should move this logics for the juernal later when i finish implementing its logics
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		FORCEINLINE	TArray<FRecordEntry> GetFinishedMissionsRecords()
	{
		return FinishedMissions;
	}

	//get the juernal for the current active user 
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
		FORCEINLINE	UAMS_JuernalObject* GetJuernal()
	{
		return JuernalSingelton;
	}

	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System | Debugging")
		FORCEINLINE	int32 GetActiveMissionsCount(FName& UsedProfile)
	{
		UsedProfile = ActiveSaveProfileName;
		return ActiveMissions.Num();
	}

	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System | Debugging")
		FORCEINLINE void LOGME()
	{
		//GetLogger()->LogSubsystem(this, 0.0f);
		FString Mes = JuernalClass ? "Valid " : "Not Valid";
		LOG_AMS(Mes);
	}
	

public:
	
	friend class AMS_LOG;

	//used to record the finished missions details for the juernal and any other use 
	void RecordMissionFinished(UMissionObject* Mission);

	void Internal_MissionSave();

	//called from outside , every mission when needs to complete any saving they call this function
	void Internal_CompleteSave(UAMS_SaveGame* saveGameObject);

	//unexposed overload for the start mission used for auto go to next mission by the system while the other one is used for in bp open mission
	void StartMission(TSubclassOf<UMissionObject> newMission);

	//get the singleton for the subsystem
	static UAMS_SubSystem* GetMissionSubSystem();

	//used to tell the data center singleton about the save process to let the user hook thier logics with this one
	void InvokeDataCenterSaveEvent(UAMS_SaveGame* saveGameObject);

	//used to generate records for not finished missions so they can be activated next time when loading
	void GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords);

	//overload to fix wrong records number, delete the other one when everything is ok
	void GenerateActiveMissionsFromRecord(UAMS_SaveGame* saveGameObject);

	//returns a records for the active missions so we can save it
	TArray<FRecordEntry> GenerateRecordsFromActiveMissions();

	inline AMS_LOG* GetLogger()
	{
		static AMS_LOG* Logger;
		if (!Logger)
		{
			Logger = new AMS_LOG();
		}

		return Logger;
	}

private:
	//used to save the missions instaces so they wont be collected by garbage collection
	TMap<TSubclassOf<UMissionObject>, UMissionObject*> ActiveMissions;

	/*Temp holder for the finished missions , use the juernal version of this one */
	TArray<FRecordEntry> FinishedMissions;

	UAMS_JuernalObject* JuernalSingelton;//change the save logics to save this one instead of the finishedMissions array
	UAMS_DataCenter* DataCenterSinglton;

	FName ActiveSaveProfileName;
	
};
