//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMS_Types.h"
#include "AMS_SubSystem.generated.h"

/**
 * 
 */
class UAMS_JuernalObject;
class UAMS_SaveGame;


static UAMS_SubSystem* MissionSubSystemInstance;



UCLASS(config = AMSSettings)
class AMS_PLUGIN_API UAMS_SubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UAMS_SubSystem();


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

	/*this function loads specific player profile data*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void LoadGame(bool& found, FName playerProfile);

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

public:
	

	//used to record the finished missions details for the juernal and any other use 
	void RecordMissionFinished(UMissionObject* Mission);

	void Internal_MissionSave();

	//unexposed overload for the start mission used for auto go to next mission by the system while the other one is used for in bp open mission
	void StartMission(TSubclassOf<UMissionObject> newMission);

	//get the singleton for the subsystem
	static UAMS_SubSystem* GetMissionSubSystem();


	//used to generate records for not finished missions so they can be activated next time when loading
	void GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords);

	//returns a records for the active missions so we can save it
	TArray<FRecordEntry> GenerateRecordsFromActiveMissions();

private:
	//used to save the missions instaces so they wont be collected by garbage collection
	TMap<TSubclassOf<UMissionObject>, UMissionObject*> ActiveMissions;

	/*Temp holder for the finished missions , use the juernal version of this one */
	TArray<FRecordEntry> FinishedMissions;

	UAMS_JuernalObject* JuernalSingelton;//change the save logics to save this one instead of the finishedMissions array

	FName ActiveSaveProfileName;
	
};
