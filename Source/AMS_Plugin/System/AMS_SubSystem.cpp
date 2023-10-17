//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_SubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Classes/GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "UObject/ConstructorHelpers.h"

#include "AMS_Plugin/Public/MissionObject.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"
#include "AMS_Plugin/Public/AMS_JuernalObject.h"
#include "AMS_Plugin/System/AMS_DataCenter.h"




UAMS_SubSystem::UAMS_SubSystem()
{
	LOG_AMS("Loaded", 10.0f, FColor::Green);

	auto FoundFadeWidgetClass = ConstructorHelpers::FClassFinder<UScreenFade>(TEXT("/AMS_Plugin/AMS_ScreenFade.AMS_ScreenFade_C"));

	if (FoundFadeWidgetClass.Class)
	{
		FadeWidgetClass = FoundFadeWidgetClass.Class;
	}

}

void UAMS_SubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	MissionSubSystemInstance = this;
	
	//InitiateFullGameProgressData();
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

	//creating the widget but we dont add it to the viewport now , we add it when we need it and never remove it from the memory just from the screen
	if (FadeWidgetClass)
	{
		FadeWidget = Cast<UScreenFade>(CreateWidget(GetWorld(), FadeWidgetClass, FName("FadeWidget")));
	}
}

void UAMS_SubSystem::Deinitialize()
{
	/*if (ActiveMissions.Num() != 0)
	ClearMissionsFromRoot();*/
}

//UWorld* UAMS_SubSystem::GetWorld() const
//{
//	if (GIsEditor && !GIsPlayInEditorWorld)
//	{
//		return nullptr;
//	}
//	else if (GetOuter())
//	{
//		return GetOuter()->GetWorld();
//	}
//
//	return nullptr;
//}

void UAMS_SubSystem::ClearMissionsFromRoot()
{
	//TArray<UMissionObject*> ActiveMissionsArray;
	//ActiveMissions.GenerateValueArray(ActiveMissionsArray);

	//for (auto& itr : ActiveMissionsArray)
	//{
	//	itr->DeInitializeMission();//unhandeled exption here, happens after long time playing and never have an active mission
	//}
}

void UAMS_SubSystem::PreformTutorialAction(TSubclassOf<UMissionObject> tutorialMission, TSubclassOf<UActionObject> tutorialAction)
{
	TSoftClassPtr<UMissionObject> temp = GetMissionSoftPtr(tutorialMission);
	if (!temp) return;

	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	PreformMissionAction(temp, tutorialAction, player);
}

#if WITH_EDITOR

void UAMS_SubSystem::RemoveMissionFromList(UObject* Mission)
{
	int32 index = 0;
	bool bFound = false;
	UAMS_SubSystem* SubSystemDefaults = Cast<UAMS_SubSystem>(UAMS_SubSystem::StaticClass()->GetDefaultObject());
	for (auto& reference : SubSystemDefaults->SoftGameMissionsList)
	{
		if (reference.GetAssetName() == Mission->GetClass()->GetName())
		{
			bFound = true;
			break;
		}

		else
		{
			LOG_AMS("Deleted Asseted check failed",10.0f, FColor::Red);
		}
		index++;
	}

	if(bFound)
	SubSystemDefaults->SoftGameMissionsList.RemoveAt(index);
}
#endif

void UAMS_SubSystem::StartMission(TSoftClassPtr<UMissionObject> newMission, FName SaveProfileName)
{	
	if (!newMission) return;

	TSubclassOf<UMissionObject> LoadedMissionClass = newMission.LoadSynchronous();

	ActiveSaveProfileName = SaveProfileName;
	if (LoadedMissionClass.GetDefaultObject()->MissionDetails.bHasCustomStartPostion)
	{
		MissionsQueue.Add(newMission);
		SetupStartupPosition();
		FADE_EXECUTE(FadeToPlay, &UAMS_SubSystem::StartQueuedMissions);
	}
	else
	{
		StartMission(newMission);
	}
	//FADE_EXECUTE(FadeToPlay, StartMission(newMission););//this now dosent call the mission from the callback

	//Internal_MissionSave();//this makes an empty save profile for this new gameplay so we can store progress directly
}

void UAMS_SubSystem::RecordMissionFinished(UMissionObject* Mission)
{
	TSoftClassPtr<UMissionObject> ActiveMissionPtr = GetMissionSoftPtr(Mission->GetClass());
	//record mission details and add the details to the finished array 
	FRecordEntry newRecord(ActiveMissionPtr, Mission->MissionDetails);

	newRecord.RequiredCount = Mission->RequiredObjectivesCount;
	newRecord.BlackListedCount = Mission->BlackListedObjectivesCount;
	newRecord.MissionDetails.MissionProgress = Mission->GetMissionProgress();
	newRecord.MissionState = Mission->CurrentState;

	FinishedMissions.Add(newRecord);//deprecated , left for near futur only, not deprecated anymore

	if(JuernalSingelton)
	JuernalSingelton->AddJuernalRecord(newRecord);

	if (ActiveMissions.Contains(Mission->GetClass()))
	{
		ActiveMissions.Remove(Mission->GetClass());
	}

	//if failed we always save , but other wise we check  if we save after mission finish
	bool SaveCondition = Mission->MissionDetails.CurrentState == EFinishState::failed ? true : (SaveType == ESaveMissionType::Post);
	if (SaveCondition)
	Internal_MissionSave();

	//Mission->RemoveFromRoot();
}

//bug : the save class is null , due to project settings are not saving or changing the subsystem cdo values
//(fixed from the properties specifires need to make them configs)
void UAMS_SubSystem::Internal_MissionSave()
{
   
	if (DataCenterSinglton)
	{
		FAMS_SavePackage SavePackage = GetSubsystemSavePackage();
		DataCenterSinglton->OnGameSaveStarted(SavePackage, ActiveSaveProfileName);
	}
	else
	{
		LOG_AMS("please provide a data center and dont forget to override the save event", 10.0f, FColor::Green);
	}
	LOG_AMS("Save Game Initiated", 10.0f, FColor::Green);
	
}

//@Bug : starting a mission that already has a finished record leaves the records , we need to delete the record before mission start
void UAMS_SubSystem::StartMission(TSoftClassPtr<UMissionObject> newMission)
{
	//if the mission was finished and we started it this time we want to wipe it's record
	WipeMissionRecord(newMission);

	TSubclassOf<UMissionObject> LoadedMissionClass = newMission.LoadSynchronous();
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	UMissionObject* StartedMission = NewObject<UMissionObject>(player, LoadedMissionClass);
	StartedMission->InitializeMission(INIT_START);

	ActiveMissions.Add(newMission,StartedMission);

	//StartedMission->AddToRoot();
	//ActiveMissions[newMission]->AddToRoot();

}


void UAMS_SubSystem::GenerateActiveMissionsFromRecord(TArray<FRecordEntry> ActiveRecords)
{
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
		UMissionObject* ActivatedMission = NewObject<UMissionObject>(this, record.MissionClass.LoadSynchronous());
		if (ActivatedMission)
		{
			LOG_AMS("Mission Activated", 10.0f, FColor::Magenta);
			ActivatedMission->MissionDetails = record.MissionDetails;
			ActivatedMission->RequiredObjectivesCount = record.RequiredCount;
			ActivatedMission->BlackListedObjectivesCount = record.BlackListedCount;

			ActivatedMission->InitializeMission(INIT_LOAD);
			ActiveMissions.Add(record.MissionClass, ActivatedMission);
			//ActivatedMission->AddToRoot();
			//ActiveMissions[record.MissionClass]->AddToRoot();
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

	//should make sure if i loaded while i have active missions like when the player pause the game and choose to load checkpoint we need to unroot missions first 
	//do this later if the game crashed after loading and playing for a bit
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
		UMissionObject* ActivatedMission = NewObject<UMissionObject>(this, record.MissionClass.LoadSynchronous());
		if (ActivatedMission)
		{
			LOG_AMS("Mission Activated", 10.0f, FColor::Magenta);
			ActivatedMission->MissionDetails = record.MissionDetails;
			ActivatedMission->RequiredObjectivesCount = record.RequiredCount;
			ActivatedMission->BlackListedObjectivesCount = record.BlackListedCount;

			ActivatedMission->InitializeMission(INIT_LOAD);
			ActiveMissions.Add(record.MissionClass, ActivatedMission);
			//ActivatedMission->AddToRoot();
			//ActiveMissions[record.MissionClass]->AddToRoot();
		}

	}

	if (!ActiveMissions.IsEmpty())
	{
		LOG_AMS("Active missions list generated from records", 10.0f, FColor::Green);
	}

	else
		LOG_AMS("Failed To Generate Acive List", 10.0f, FColor::Red);
}


void UAMS_SubSystem::__FadeAndExecute(ScreenFadeType Type, TFunction<void(void)>&& Callback)
{

	FadeWidget = Cast<UScreenFade>(CreateWidget(GetWorld(), FadeWidgetClass, FName("FadeWidget")));
	FadeWidget->AddToViewport(100);//to come above all ui 
	
	float length = FadeWidget->RunFadeToPlay();
	
	FTimerHandle FadeHandle;
	GetWorld()->GetTimerManager().SetTimer(FadeHandle,this, &UAMS_SubSystem::StartQueuedMissions, length, false);
}

void UAMS_SubSystem::__FadeAndExecute(ScreenFadeType Type, typename FTimerDelegate::TMethodPtr<UAMS_SubSystem> Callback)
{
	FadeWidget = Cast<UScreenFade>(CreateWidget(GetWorld(), FadeWidgetClass, FName("FadeWidget")));
	FadeWidget->AddToViewport(100);//to come above all ui 

	float length = Type == ScreenFadeType::FadeToPlay? FadeWidget->RunFadeToPlay() : FadeWidget->RunPlayToFade();

	FTimerHandle FadeHandle;
	GetWorld()->GetTimerManager().SetTimer(FadeHandle, this, Callback, length, false);
}

void UAMS_SubSystem::SetupStartupPosition()
{
	if (MissionsQueue.Num() > 0)
	{
		//int32 lastIndex = MissionsQueue.Num() - 1;
		//StartTransform = MissionsQueue[lastIndex].GetDefaultObject()->MissionDetails.StartTransform;

		//UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorTransform(StartTransform);
	}
}

void UAMS_SubSystem::StartQueuedMissions()
{
	for (auto& mission : MissionsQueue)
	{
		StartMission(mission);
	}
	MissionsQueue.Empty();
}

TArray<FRecordEntry> UAMS_SubSystem::GenerateRecordsFromActiveMissions()
{
	TArray<FRecordEntry> Records;
	for (auto& kvPair : ActiveMissions)
	{
		int32 required = kvPair.Value->RequiredObjectivesCount;
		int32 blacklisted = kvPair.Value->BlackListedObjectivesCount;

		kvPair.Value->UpdateCounterInDetails();//copy the counter inside the details so it get saved with it for use later after loading 

		kvPair.Value->GetMissionProgress();//this will calculate the current progress inside the details so when we save it to  the record we get the currect value

		Records.Emplace(kvPair.Key, kvPair.Value->MissionDetails, kvPair.Value->CurrentState, required, blacklisted);	
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
		 JuernalSingelton->AssignRecordList(FinishedMissions);
	}
}

UAMS_SubSystem* UAMS_SubSystem::GetMissionSubSystem()
{
	return MissionSubSystemInstance;
}


void UAMS_SubSystem::InvokeDataCenterLoadEvent(UAMS_SaveGame* saveGameObject)
{
	if (DataCenterSinglton)
	{
		DataCenterSinglton->OnGameLoaded(saveGameObject);
	}
	
}


void UAMS_SubSystem::PreformMissionAction(TSoftClassPtr<UMissionObject> Mission, TSubclassOf<UActionObject> PreformedAction, AActor* ActionSource)
{
	UActionObject* preformedActionCDO = PreformedAction.GetDefaultObject();

	if (ActiveMissions.Contains(Mission) && IsValid(ActionSource))
	{
		if (!ActionSource->ActorHasTag(ActiveMissions[Mission]->AssossiatedTag)) return;

		FObjective& objective = ActiveMissions[Mission]->MissionDetails.GetActionRelatedObjective(PreformedAction);
		if (objective.SoftActionClass && !objective.bIsFinished)
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

void UAMS_SubSystem::AssossiateActor(AActor* Actor, TSoftClassPtr<UMissionObject> forMission)
{
	if (!Actor || !forMission) return;

	if (ActiveMissions.Contains(forMission))
	{
		ActiveMissions[forMission]->AssossiateActor(Actor);
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
	if(!CheckPointMissionsRecords.IsEmpty())
	GenerateActiveMissionsFromRecord(CheckPointMissionsRecords);
}

void UAMS_SubSystem::CancelMission(TSoftClassPtr<UMissionObject> mission)
{
	if (ActiveMissions.Contains(mission))
	{
		ActiveMissions[mission]->EndMission(EFinishState::canceled, FFailInfo(EMissionFailReason::canceled));
	}
}

void UAMS_SubSystem::RestartMission(TSoftClassPtr<UMissionObject> mission, ERestartType restartType)
{
	if (!mission) return;
	
	WipeMissionRecord(mission);

	if (!ActiveMissions.Contains(mission))
	{
		StartNewMission(mission, ActiveSaveProfileName);
		return;
	}

	ActiveMissions[mission]->CurrentState = EFinishState::canceled;//just to prevent it from ticking but it is not recorded since this was direct set for the state
	ActiveMissions.Remove(mission);//remove it and leave any other active quests
	switch (restartType)
	{
	case ERestartType::fromCheckPoint:

		LoadCheckPoint();
		break;

	case ERestartType::fromStart:

		StartNewMission(mission, ActiveSaveProfileName);
		break;

	default:

		StartNewMission(mission, ActiveSaveProfileName);
		break;
	}
}

void UAMS_SubSystem::PauseMission(TSoftClassPtr<UMissionObject> mission, bool IsPaused)
{
	if (ActiveMissions.Contains(mission))
	{
		ActiveMissions[mission]->PauseMission(IsPaused);
	}
}

float UAMS_SubSystem::Internal_GetGameProgress()
{
	LOG_AMS("Game Progress Is Being Called");
	float progress = 0.0f;
	for (auto& itr : FinishedMissions)
	{
		float next;
	   itr.IsFinished(next);
	   progress += next;//increment now here for more straight logic
	}

	return progress / static_cast<float>(FullGameMissionsCount);
}

//called once on begin play so that the game knows the count of games missions 
//@Bug : asset registry is not working on build
//@worked around using mission tweaker to do this in editor time on demand so no need to load on module start
void UAMS_SubSystem::InitiateFullGameProgressData()
{
	FullGameMissionsRecords.Empty();
	FullGameMissionsCount = 0;
	
	FullGameMissionsCount = SoftGameMissionsList.Num();

	for (auto& mission : SoftGameMissionsList)
	{
		UMissionObject* missionObject = Cast<UMissionObject>(mission.LoadSynchronous()->GetDefaultObject());
		if (missionObject)
		{
			FMissionDetails recordDetails = missionObject->MissionDetails;
			recordDetails.MissionRelatedActions = AMS_Types::GenerateDetails(missionObject->MissionRelatedActions).MissionRelatedActions;
			FRecordEntry newRecord = FRecordEntry(missionObject->GetClass(), recordDetails);
			newRecord.MissionState = EFinishState::notPlayed;
			FullGameMissionsRecords.Add(newRecord);
		}
	}

}

