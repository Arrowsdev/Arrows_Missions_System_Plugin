//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMS_Types.h"
#include "SScreenFade.h"
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

	TSharedPtr<SScreenFade> ScreenFade;
	TSharedPtr<class SWidget> Container;

//PROJECT SETTINGS

    //the mission class that will be the first one \n if null you will have to manually call start mission when ever you want to start one
	UPROPERTY(config,EditAnywhere, Category = "Settings")
		TSubclassOf<UMissionObject> StartupMission;

	//the juernal class used to record progress and achivements
	UPROPERTY(config,EditAnywhere, Category = Settings)
		TSubclassOf<UAMS_JuernalObject> JuernalClass;

	//this is the list of all playable missins in the game , we use this to get the game compeletion only it dosnt affect
	//the order in which the mission are played since we manually play missins by start mission call, if filled it will override the default behaviour
	//were we consider all mission classes in the project, but if this filled with one element it will be our full game list
	UPROPERTY(config, EditAnywhere, Category = "Settings")
		TArray<TSubclassOf<UMissionObject>> GameMissionsList;

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

	//enable or disable screen logs, yet the consol log will always be present
	UPROPERTY(config, EditAnywhere, Category = "Settings")
		bool AllowScreenDebug;

// END OF PROJECT SETTINGS

	/*starts new mission */
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
    void StartMission(TSubclassOf<UMissionObject> newMission, FName SaveProfileName);

	/*Starts New Mission*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static FORCEINLINE void StartNewMission(TSubclassOf<UMissionObject> newMission, FName SaveProfileName)
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->StartMission(newMission, SaveProfileName);
	}

	//preform an action for specific mission from the active missions , if the provided mission is not active
	//the call will be ignored, the source should also have a mission associated tag for it action to be considered
	//so if killing enemies was an action we want only those who has the tag to be counted so not all enemies are counted 
	// you need to add this tag on spawn for your enemies to distinguish the default enemy class from the enemy that was spawned 
	//for this passed mission
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	void PreformMissionAction(TSubclassOf<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction, AActor* ActionSource);
	
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static FORCEINLINE void PreformAction(TSubclassOf<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction, AActor* ActionSource)
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->PreformMissionAction(Mission, PreformedAction, ActionSource);
	}

	//this function creates a savegame object , if there was an old profile it loads the last progress and let you 
	//hook your new data too before saving the file again
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	UAMS_SaveGame* CreateMissionSaveObject();

	/*this function loads specific player profile data, returns a reference to the loaded object
	* so you can load the custom data you have saved
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		UAMS_SaveGame* LoadGame(FName playerProfile, bool& found);

	/*this function loads specific player profile data, returns a reference to the loaded object
	* so you can load the custom data you have saved
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta=(DisplayName="Load Game"))
		static FORCEINLINE UAMS_SaveGame* ST_LoadGame(FName playerProfile, bool& found)//ST = static
	{
		if (!MissionSubSystemInstance) return nullptr;
		return MissionSubSystemInstance->LoadGame(playerProfile, found);
	}

	/*used to save game at desired time and not on the subsystem times
	* dont forget to use the data center to put the data you want to save 
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		FORCEINLINE	void SaveGame() 
	{
		Internal_MissionSave();
	}

	/*used to save game at desired time and not on the subsystem times
	* dont forget to use the data center to put the data you want to save
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta = (DisplayName = "Save Game"))
	static FORCEINLINE void ST_SaveGame()
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->SaveGame();
	}

	/*used to create a check point for missions to restart from for the active player profile*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void CreateCheckPoint();

	/*used to load last created checkpoint for the active player profile*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void LoadCheckPoint();

	/*used to cancel the mission , it will be added to the finished missions 
	* and marked as failed 
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void CancelMission(TSubclassOf<UMissionObject> mission);

	/*used to restart any mission if it was active or recently failed, and if not it will start the mission*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void RestartMission(TSubclassOf<UMissionObject> mission, ERestartType restartType);

	//called to pause a mission by it class if it was running 
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void PauseMission(TSubclassOf<UMissionObject> mission, UPARAM(DisplayName = "Pause?") bool IsPaused);

	/*used to cancel the mission , it will be added to the finished missions
	* and marked as failed
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta = (DisplayName = "Cancel Mission"))
	static FORCEINLINE void ST_CancleMission(TSubclassOf<UMissionObject> mission)
	{
		if (!MissionSubSystemInstance)return;
		return MissionSubSystemInstance->CancelMission(mission);
	}

	//get the records for the finished missions , should move this logics for the juernal later when i finish implementing its logics
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		FORCEINLINE	TArray<FRecordEntry> GetFinishedMissionsRecords()
	{
		return FinishedMissions;
	}

	//get currently active missions with their informations 
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static	FORCEINLINE	TArray<FRecordEntry> GetActiveMissions()
	{
		if (!MissionSubSystemInstance)return TArray<FRecordEntry>();
		return MissionSubSystemInstance->GenerateRecordsFromActiveMissions();
	}

	//get the juernal for the current active user 
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE	UAMS_JuernalObject* GetJuernal()
	{
		if (!MissionSubSystemInstance)return nullptr;
		return MissionSubSystemInstance->JuernalSingelton;
	}

	/*parse the passed objective to string, the struct is objective but it counts for blacklisted actions and also required or optional actions
	* i hope it is not confusing using one struct for all of them , this way is cleaner , the type can be used in ui for the displayed info
	*/
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE FString ParseObjective(FObjective objective, EStatusGetterType getterType, FString& ObjectiveType)
	{
		return objective.GetObjectibeStatus(getterType, ObjectiveType);
	}

	/*get mission refernce if it was active so you can get   global access to it's properties and functions*/
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE UMissionObject* GetMissionByClass(TSubclassOf<UMissionObject> missionClass)
	{
		if (!MissionSubSystemInstance) return nullptr;
		return MissionSubSystemInstance->ActiveMissions[missionClass];
	}

	//get all game missins records so you can use them to create a menu that the player can select a mission to play, or any other use
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static FORCEINLINE TArray<FRecordEntry> GetGameMissions()
	{
		if (!MissionSubSystemInstance)return TArray<FRecordEntry>();
		return MissionSubSystemInstance->FullGameMissionsRecords;
	}

	/*get the full game progress*/
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static FORCEINLINE float GetGameProgress()
	{
		if (!MissionSubSystemInstance)return 0.0f;
		return MissionSubSystemInstance->Internal_GetGameProgress();
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
		GetLogger()->LogSubsystem(this, 0.0f);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System | Debugging")
		FORCEINLINE void Fade()
	{
		if (GEngine->GameViewport)
		{
			ScreenFade = SNew(SScreenFade);
			GEngine->GameViewport->AddViewportWidgetContent(ScreenFade.ToSharedRef());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Widget Created!");
		}
	}

public:
	
	friend class AMS_LOG;

	//used to record the finished missions details for the juernal and any other use 
	void RecordMissionFinished(UMissionObject* Mission);

	void Internal_MissionSave();

	//called from the data center if the user overrided the on save process started event
	void Internal_CompleteSave(UAMS_SaveGame* saveGameObject);

	//unexposed overload for the start mission used for auto go to next mission by the system while the other one is used for in bp open mission
	void StartMission(TSubclassOf<UMissionObject> newMission);

	//get the singleton for the subsystem
	static UAMS_SubSystem* GetMissionSubSystem();

	//used to tell the data center singleton about the save process to let the user hook thier logics with this one
	void InvokeDataCenterSaveEvent(UAMS_SaveGame* saveGameObject);

	//used to tell the data center about the loading 
	void InvokeDataCenterLoadEvent(UAMS_SaveGame* saveGameObject);

	//used to generate records for not finished missions so they can be activated next time when loading
	void GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords);

	//overload to fix wrong records number, delete the other one when everything is ok
	void GenerateActiveMissionsFromRecord(UAMS_SaveGame* saveGameObject);

	//used to calculate the progress for the whole game 
	float Internal_GetGameProgress();

	void InitiateFullGameProgressData();


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

	//used to return either the saved finished missions from the juernal or use the one in the subsystem
	TArray<FRecordEntry>& GetFinishedMissions();
	
	void LoadFinishedMissionsToJuernal();
	
private:
	//used to save the missions instaces so they wont be collected by garbage collection
	//(funny thing that the list it self wasn't safe from it untill we decorate it with the property macro XDD)
	UPROPERTY()
	TMap<TSubclassOf<UMissionObject>, UMissionObject*> ActiveMissions;

	/*Temp holder for the finished missions , use the juernal version of this one , not temp anymore used to have internal records if we dont  have a juernal*/
	UPROPERTY()
	TArray<FRecordEntry> FinishedMissions;

	//a records list for missions states and progress before any mission fail , used for restart game from check point option.
	UPROPERTY()
	TArray<FRecordEntry> CheckPointMissionsRecords;

	//records for missions information it will be modified manually to get 100% compeletion from them and this way we get 
	//pretty good data to represent for the player instead of just the amount of missions left
	UPROPERTY()
	TArray<FRecordEntry> FullGameMissionsRecords;

	UPROPERTY()
	UAMS_JuernalObject* JuernalSingelton;//change the save logics to save this one instead of the finishedMissions array

	UPROPERTY()
	UAMS_DataCenter* DataCenterSinglton;

	UPROPERTY()
	FName ActiveSaveProfileName;
	
	//the mission count that is used to find how much of the game is finished
	int32 FullGameMissionsCount = INDEX_NONE;
};
