//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AMS_Types.h"
#include "SScreenFade.h"
#include "AMS_SubSystem.generated.h"


//macro to simplfy calling short logics with the fade function 
#define FADE_EXECUTE(FadeType, CallBack )\
__FadeAndExecute(FadeType, CallBack )

//for large code blocks to wait for fade widget we use these two macros , one before the block and other after it
//the block will be executed after the fade animation is finished
#define START_FADE_BLOCK(FadeType,...)\
__FadeAndExecute(FadeType, [&, __VA_ARGS__]() { 
#define END_FADE_BLOCK });

/**
 * subsystem class that has all the core logics and it is the main interface to control your missions behaviours 
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
	
	virtual void Deinitialize() override;

	//UWorld* GetWorld() const;

	UPROPERTY(config, EditAnywhere, Category = "Settings")
		TSubclassOf<UScreenFade> FadeWidgetClass;

	UPROPERTY()
		UScreenFade* FadeWidget;

	//enum to define how we use the screen fade widget
	enum ScreenFadeType
	{
		FadeToPlay,
		PlayToFade
	};

	//enum to define how the profile play time is called and where
	enum ProfileCase
	{
		Created,
		Loaded,
		Ended
	};

//PROJECT SETTINGS

    //the mission class that will be the first one \n if null you will have to manually call start mission when ever you want to start one
	UPROPERTY(config,EditAnywhere, Category = "Settings")
		TSubclassOf<UMissionObject> StartupMission;

	//the juernal class used to record progress and achivements
	UPROPERTY(config,EditAnywhere, Category = Settings)
		TSubclassOf<UAMS_JuernalObject> JuernalClass;

	////this is the list of all playable missins in the game , we use this to get the game compeletion only it dosnt affect
	////the order in which the mission are played since we manually play missins by start mission call, if filled it will override the default behaviour
	////were we consider all mission classes in the project, but if this filled with one element it will be our full game list
	//UPROPERTY(config, EditAnywhere, Category = "Settings")
	//	TArray<TSubclassOf<UMissionObject>> GameMissionsList;

	//this is the list of all playable missins in the game , we use this to get the game compeletion only it dosnt affect
	//the order in which the mission are played since we manually play missins by start mission call, if filled it will override the default behaviour
	//were we consider all mission classes in the project, but if this filled with one element it will be our full game list
	UPROPERTY(config, VisibleAnywhere, Category = "Settings" ,meta=(DisplayName="GameMissionsList"))
		TArray<TSoftClassPtr<UMissionObject>> SoftGameMissionsList;

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

	/*starts new mission;
	 @newMission %the new mission class to start;
	 @SaveProfileName %the name of the new player's profile name;
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
    void StartMission(TSoftClassPtr<UMissionObject> newMission, FName SaveProfileName);

	/*starts new mission;
	 @newMission %the new mission class to start;
	 @SaveProfileName %the name of the new player's profile name;
	*/
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static FORCEINLINE void StartNewMission(TSoftClassPtr<UMissionObject> newMission, FName SaveProfileName)
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->StartMission(newMission, SaveProfileName);
	}

	//preform an action for specific mission from the active missions , if the provided mission is not active
	//the call will be ignored, the source should also have a mission associated tag for it action to be considered
	//so if killing enemies was an action we want only those who has the tag to be counted so not all enemies are counted 
	// you need to add this tag on spawn for your enemies to distinguish the default enemy class from the enemy that was spawned 
	//for this passed mission;
	//@Mission %The Mission That Has This Action As Related Action.;
	//@PreformedAction %The Related Action That Was Preformed!.;
	//@ActionSource %The Source Of This Action, The Actor That Did This Action;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	void PreformMissionAction(TSoftClassPtr<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction, AActor* ActionSource);
	

	//preform an action for specific mission from the active missions , if the provided mission is not active
	//the call will be ignored, the source should also have a mission associated tag for it action to be considered
	//so if killing enemies was an action we want only those who has the tag to be counted so not all enemies are counted 
	// you need to add this tag on spawn for your enemies to distinguish the default enemy class from the enemy that was spawned 
	//for this passed mission;
	//@Mission %The Mission That Has This Action As Related Action.;
	//@PreformedAction %The Related Action That Was Preformed!.;
	//@ActionSource %The Source Of This Action, The Actor That Did This Action;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static FORCEINLINE void PreformAction(TSoftClassPtr<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction, AActor* ActionSource)
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->PreformMissionAction(Mission, PreformedAction, ActionSource);
	}

	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		static FORCEINLINE void PreformAction2(TSoftClassPtr<UMissionObject> Mission, TSoftClassPtr<UActionObject> PreformedAction, AActor* ActionSource)
	{
		//
	}


	//this function creates a savegame object , if there was an old profile it loads the last progress and let you 
	//hook your new data too before saving the file again [Deprecated Function];
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	UAMS_SaveGame* CreateMissionSaveObject();

	//this function loads specific player profile data, returns a reference to the loaded object
	//so you can load the custom data you have saved;
	//@PlayerProfile %The Name Of The New Profile To Load.;
	//@Found %If The Profile Was Found Or Not!.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		UAMS_SaveGame* LoadGame(FName playerProfile, bool& found);

	//this function loads specific player profile data, returns a reference to the loaded object
	//so you can load the custom data you have saved;
	//@PlayerProfile %The Name Of The New Profile To Load.;
	//@Found %If The Profile Was Found Or Not!.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta=(DisplayName="Load Game"))
		static FORCEINLINE UAMS_SaveGame* ST_LoadGame(FName playerProfile, bool& found)//ST = static
	{
		if (!MissionSubSystemInstance) return nullptr;
		return MissionSubSystemInstance->LoadGame(playerProfile, found);
	}

	//used to save game at desired time and not on the subsystem times
	//dont forget to use the data center to put the data you want to save;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		FORCEINLINE	void SaveGame() 
	{
		Internal_MissionSave();
	}

	//global version of setting actors assossiated with any of the active missions;
	//@Actor %The Actor That Should be Assossiated To The Provided Mission.;
	//@ForMission %This is The Provided Mission.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void  AssossiateActor(AActor* Actor, TSoftClassPtr<UMissionObject> forMission);
	

	//used to save game at desired time and not on the subsystem times
	//dont forget to use the data center to put the data you want to save;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta = (DisplayName = "Save Game"))
	static FORCEINLINE void ST_SaveGame()
	{
		if (!MissionSubSystemInstance) return;
		MissionSubSystemInstance->SaveGame();
	}

	//used to create a check point for missions to restart from for the active player profile;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void CreateCheckPoint();

	//used to load last created checkpoint for the active player profile;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void LoadCheckPoint();

	//used to cancel the mission , it will be added to the finished missions 
	//and marked as failed;
	//@Mission %The Mission To Cancel.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void CancelMission(TSoftClassPtr<UMissionObject> mission);

	//used to restart any mission if it was active or recently failed, and if not it will start the mission;
	//@Mission %The Mission To Restart.;
	//@RestrtType %How To Restart, From Restart? Or From Checkpoint?.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void RestartMission(TSoftClassPtr<UMissionObject> mission, ERestartType restartType);

	//called to pause a mission by it class if it was running;
	//@Mission %The Mission To Pause or Unpause.;
	//@Pause? %Should Pause Or Unpause.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		void PauseMission(TSoftClassPtr<UMissionObject> mission, UPARAM(DisplayName = "Pause?") bool IsPaused);

	//used to cancel the mission , it will be added to the finished missions
	//and marked as failed;
	//@Mission %The Mission To Cancel.;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System", meta = (DisplayName = "Cancel Mission"))
	static FORCEINLINE void ST_CancleMission(TSoftClassPtr<UMissionObject> mission)
	{
		if (!MissionSubSystemInstance)return;
		return MissionSubSystemInstance->CancelMission(mission);
	}

	//get the records for the finished missions , should move this logics for the juernal later when i finish implementing its logics;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
		FORCEINLINE	TArray<FRecordEntry> GetFinishedMissionsRecords()
	{
		return FinishedMissions;
	}

	//get currently active missions with their informations;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System")
	static	FORCEINLINE	TArray<FRecordEntry> GetActiveMissions()
	{
		if (!MissionSubSystemInstance)return TArray<FRecordEntry>();
		return MissionSubSystemInstance->GenerateRecordsFromActiveMissions();
	}

	//get the juernal for the current active user;
	//@NaN;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE	UAMS_JuernalObject* GetJuernal()
	{
		if (!MissionSubSystemInstance)return nullptr;
		return MissionSubSystemInstance->JuernalSingelton;
	}

	//parse the passed objective to string, the struct is objective but it counts for blacklisted actions and also required or optional actions
	//i hope it is not confusing using one struct for all of them , this way is cleaner , the type can be used in ui for the displayed info.;
	//@Objective %The Objective To Parse.;
	//@GetterType %The Format In Which The Objective String Will Be Returned In!.;
	//@ObjectiveType %The Type Of The Objective , Required Or Other Types.;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE FString ParseObjective(FObjective objective, EStatusGetterType getterType, FString& ObjectiveType)
	{
		return objective.GetObjectibeStatus(getterType, ObjectiveType);
	}

	//get mission refernce if it was active so you can get   global access to it's properties and functions.;
	//@MissionClass %The Class Of The Mission You Want It's Instance From The Active List If Found.;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static	FORCEINLINE UMissionObject* GetMissionByClass(TSoftClassPtr<UMissionObject> missionClass)
	{
		if (!MissionSubSystemInstance) return nullptr;
		return MissionSubSystemInstance->ActiveMissions[missionClass];
	}

	//get all game missins records so you can use them to create a menu that the player can select a mission to play, or any other use.;
	//@NaN;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static FORCEINLINE TArray<FRecordEntry> GetGameMissions()
	{
		if (!MissionSubSystemInstance)return TArray<FRecordEntry>();
		return MissionSubSystemInstance->FullGameMissionsRecords;
	}

	//get the full game progress, meaning how much of the game missions the player has finished taking into account
	//side missions too even they are not needed to complete the game but they participate in the compeletion precentage.;
	//@NaN;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static FORCEINLINE float GetGameProgress()
	{
		if (!MissionSubSystemInstance)return 0.0f;
		return MissionSubSystemInstance->Internal_GetGameProgress();
	}

	//check if a record is played or fresh not played, used to help making ui for playing games from the list of the game missions
	//since there my example ui used to restart all missions when "play this mission" button is pressed but restarting a mission without having playing any mission
	//gives the player a default profile of "NONE" so it saves in the wrong place, this is just a utility function, i think we should just expose the enum entry
	//and remove this funciton, but until then this is how you know if a certain record is played or recently generated;
	//@Record %The Record To Check.;
    //@Played? %If Played Or Not.;
	UFUNCTION(BlueprintPure, Category = "Arrows Mission System")
	static FORCEINLINE void GetRecordState(FRecordEntry record, UPARAM(DisplayName="played?") bool& bIsPlayed)
	{
		bIsPlayed = record.MissionState != EFinishState::notPlayed;
	}

	//A Debugging Function To Get how many active missions there is;
	//@UsedProfile %The Loaded Profile To See How Many Active Missions In, [Must Be Loaded!];
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System | Debugging")
		FORCEINLINE	int32 GetActiveMissionsCount(FName& UsedProfile)
	{
		UsedProfile = ActiveSaveProfileName;
		return ActiveMissions.Num();
	}

	//Logger Function to Print Some Data To The consol , you dont need to touch any of the debug functions anywhere unless you are 
	//using cpp to expand the system, taking in mind the LOG class is still not finished and it will be compeleted in the future;
	//@NaN;
	UFUNCTION(BlueprintCallable, Category = "Arrows Mission System | Debugging")
		FORCEINLINE void LOGME()
	{
		GetLogger()->LogSubsystem(this, 0.0f);
	}
	
public:
	
	friend class AMS_LOG;

	//used to record the finished missions details for the juernal and any other use 
	void RecordMissionFinished(UMissionObject* Mission);

	void Internal_MissionSave();

	//unexposed overload for the start mission used for auto go to next mission by the system while the other one is used for in bp open mission
	void StartMission(TSoftClassPtr<UMissionObject> newMission);

	//get the singleton for the subsystem
	static UAMS_SubSystem* GetMissionSubSystem();

	//for new save logics
	static inline TSubclassOf<UAMS_SaveGame> GetSaveGameClass()
	{
		if(MissionSubSystemInstance)
		return MissionSubSystemInstance->SaveGameClass;

		return nullptr;
	}
	static inline FName GetSaveProfile()
	{
		if (MissionSubSystemInstance)
			return MissionSubSystemInstance->ActiveSaveProfileName;

		return FName();
	}

	//used to tell the data center about the loading 
	void InvokeDataCenterLoadEvent(UAMS_SaveGame* saveGameObject);

	//used to generate records for not finished missions so they can be activated next time when loading
	void GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords);

	//overload to fix wrong records number, delete the other one when everything is ok
	void GenerateActiveMissionsFromRecord(UAMS_SaveGame* saveGameObject);

	//used to calculate the progress for the whole game 
	float Internal_GetGameProgress();

	void InitiateFullGameProgressData();//still need to find a way to avoid crashing in editor when calling the AMS operations to create details in editor time

	//should only pass lambdas so we can pass different parameters in the capture
	void __FadeAndExecute(ScreenFadeType Type, TFunction<void(void)>&& Callback);

	void __FadeAndExecute(ScreenFadeType Type, typename FTimerDelegate::TMethodPtr< UAMS_SubSystem > Callback);

	//used when starting queued missoins to set the location while the screen is black so the player wont see the transition
	void SetupStartupPosition();

	//this will be called after the fade since we are losing the reference of the started mission while waiting for the 
	//fade i though it is a better idea to store a copy of the argument in the queue list and start them directly when the time comes
	void StartQueuedMissions();

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
	
	//used to un root all missions pointers so the game can clear memeory when it's closing otherwise it will crash
	void ClearMissionsFromRoot();

	void PreformTutorialAction(TSubclassOf<UMissionObject> tutorialMission, TSubclassOf<UActionObject> tutorialAction);

	//get the mission soft ptr from the active missions list , used to help with functions that passes TSubclassOf
	inline TSoftClassPtr<UMissionObject> GetMissionSoftPtr(TSubclassOf<UMissionObject> mission)
	{
		for (auto& itr : ActiveMissions)
		{
			if (itr.Key.GetAssetName() == mission->GetName())
			{
				return itr.Key;
			}
		}

		return  nullptr;
	}

	//to work around some execptions errors
	inline FAMS_SavePackage GetSubsystemSavePackage()
	{
		FAMS_SavePackage package;
		package.SG_ActiveMissionsWhenSaved = GenerateRecordsFromActiveMissions();
		package.SG_CheckPointMissionsRecords = CheckPointMissionsRecords;
		package.SG_FinishedMissions = GetFinishedMissions();
		return package;
	}

	//removes mission record, used if we started a mission that has a finished record we wipe the old record and start the mission so they dont pack up
	inline void WipeMissionRecord(TSoftClassPtr<UMissionObject> mission)
	{
		int32 index = -1;
		TArray<int32> DirtyIndexes;
		//store dirty indexes for removal
		for (auto& _FinishedMission : FinishedMissions)
		{
			index++;
			if (_FinishedMission == mission)
			{
				DirtyIndexes.Add(index);
			}
		}
		///remove dirty
		for (auto& _index : DirtyIndexes)
		{
			FinishedMissions.RemoveAt(_index);
		}
	}

	//called when ever a profile is created or loaded
	//we need to work around that our start new mission logics , it is not ideal for this functin to be called there
	//@ToDo : finish this logic later
	inline void CalcProfilePlayTime(ProfileCase _ProfileCase, float LastPlayTime = 0)
	{
		switch (_ProfileCase)
		{
			case Created :
			{
				TotalPlayTime = 0;
				GameStartTime = FPlatformTime::Seconds();
			}
			case Loaded:
			{
				TotalPlayTime = LastPlayTime;
				GameStartTime = FPlatformTime::Seconds();
			}

			case Ended:
			{
				TotalPlayTime = FPlatformTime::Seconds() - GameStartTime;
			}

		}
	}

#if WITH_EDITOR
	
	inline void AddMissionToList(TSoftClassPtr<UMissionObject> newMissionAsset)
	{
		SoftGameMissionsList.Add(newMissionAsset);
	}

	void RemoveMissionFromList(UObject* Mission);

	inline void ResetMissionsList()
	{
		SoftGameMissionsList.Empty();
	}
	
	inline void UpdateFullGameMissionsList(TArray<FRecordEntry> newList)
	{
		FullGameMissionsRecords = newList;
		FullGameMissionsCount = newList.Num();
	}

#endif	

private:
	//used to save the missions instaces so they wont be collected by garbage collection
	//(funny thing that the list it self wasn't safe from it untill we decorate it with the property macro XDD)
	UPROPERTY()
	TMap<TSoftClassPtr<UMissionObject>, UMissionObject*> ActiveMissions;

	//list of missions waiting for fade to finish so they can start
	UPROPERTY()
	TArray<TSoftClassPtr<UMissionObject>> MissionsQueue;

	/*Temp holder for the finished missions , use the juernal version of this one , not temp anymore used to have internal records if we dont  have a juernal*/
	UPROPERTY()
	TArray<FRecordEntry> FinishedMissions;

	//a records list for missions states and progress before any mission fail , used for restart game from check point option.
	UPROPERTY()
	TArray<FRecordEntry> CheckPointMissionsRecords;

	//records for missions information it will be modified manually to get 100% compeletion from them and this way we get 
	//pretty good data to represent for the player instead of just the amount of missions left
	UPROPERTY(config)
	TArray<FRecordEntry> FullGameMissionsRecords;

	UPROPERTY()
	UAMS_JuernalObject* JuernalSingelton;//change the save logics to save this one instead of the finishedMissions array

	UPROPERTY()
	UAMS_DataCenter* DataCenterSinglton;

	UPROPERTY()
	FName ActiveSaveProfileName;
	
	//used to set the start transform for the player, it will be set in any new mission start
	//or checkPoint creation, or savegame , and used when loading game with any type (from start or checkpoint)
	UPROPERTY()
	FTransform StartTransform;

	//not sure if i should worry about the level name and should let the user open the map he knows that the mission is  from
	//but  i'll leave it to make room for expansion in this direction in the future if needed
	UPROPERTY()
	FName PlayingLevel;

	//the amount of time in seconds played in certain profile
	UPROPERTY()
	float  TotalPlayTime;

	//this is the time when the game is started for the profile either loaded or created , so when we close we compare to platform time and 
	//count how much time elapsed playing
	UPROPERTY()
	float GameStartTime;

	//the mission count that is used to find how much of the game is finished
	UPROPERTY(config)
	int32 FullGameMissionsCount = INDEX_NONE;
};
