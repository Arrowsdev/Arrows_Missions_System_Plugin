//Advanced Mission System Developed by AIS Studios @2023


#include "MissionObject.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"


UMissionObject::UMissionObject()
{
	LOG_AMS("Constrcut new Mission", 20.0f);
	CurrentState = EFinishState::notPlayed;
	AssossiatedTag = this->GetClass()->GetFName();
}

UMissionObject::~UMissionObject()
{
	LOG_AMS("Mission Has been Distructed", 20.0f);
}

void UMissionObject::Tick(float DeltaTime)
{
	LOG_AMS("still active", 0.0f, FColor::Green);
	//only tick if in progress
	if (CurrentState != EFinishState::inProgress) return;
	

	MissionTick(DeltaTime);
	CountTime(DeltaTime);
	MissionTickDelegate.Broadcast(DeltaTime);
	LOG_AMS("Mission Is In Progress", 0.0f);
}

TStatId UMissionObject::GetStatId() const
{
	return UObject::GetStatID();
}

bool UMissionObject::IsTickable() const
{
	//only tick if we are not the default object CDO //this != GetClass()->GetDefaultObject() && 
	return (CurrentState == EFinishState::inProgress);
}

UWorld* UMissionObject::GetWorld() const
{
#if WITH_EDITOR
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

#endif
	const UObject* Outer = GetOuter();
	if (Outer == nullptr)
	{
		return nullptr;
	}
	return Outer->GetWorld();
}

void UMissionObject::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	FAssetRegistryTag MissionTag = FAssetRegistryTag(FName("MissionObject"), UMissionObject::StaticClass()->GetName(), FAssetRegistryTag::TT_Alphabetical);
	OutTags.Add(MissionTag);
}

void UMissionObject::InitializeMission(bool bStart)
{
	TMap<TSoftClassPtr<UActionObject>, int32> InstancesMap;

	if (!bStart) goto FinishInit;

    /*count action instances and increment by action count*/
	for (auto& itr : MissionRelatedActions)
	{
		TSubclassOf<UActionObject> ActionClass = itr.LoadSynchronous();
		UActionObject* ActionCDO = ActionClass.GetDefaultObject();

		if (InstancesMap.Contains(itr) && ActionCDO->ActionType != EActionType::InputListener)
		{
			InstancesMap[itr]+= ActionCDO->LocalActionCount;
		}

		else if(!InstancesMap.Contains(itr))//so we dont double counting the input listeners, and make sure we only add new entries 
		{
			InstancesMap.Add(itr, ActionCDO->LocalActionCount);

			//inputs are considered required since they are required for tutorials to finish
			if (ActionCDO->ActionType == EActionType::required || ActionCDO->ActionType == EActionType::InputListener)
				RequiredObjectivesCount++;

			if (ActionCDO->ActionType == EActionType::blacklisted)
				BlackListedObjectivesCount++;
		}
	}

	for (auto& kv : InstancesMap)
	{
		MissionDetails.MissionRelatedActions.Emplace(kv.Key, kv.Value, this);
	}

FinishInit:
	MissionDetails.RequiredCount = RequiredObjectivesCount;
	MissionDetails.InitOptionalCout();
	MissionDetails.ActivateActions(this);
	TimeCounter = MissionDetails.MissionTimeCounter;
	CurrentState = EFinishState::inProgress;
	OnMissionBegin();
	MissionHasUpdated();
	LOG_AMS("Mission is initilized", 10.0f, FColor::Green);
}


void UMissionObject::EndMission(EFinishState finishState, FFailInfo FailInfo)
{
	CurrentState = finishState;
	MissionDetails.CurrentState = finishState;

	//seek to evaluate On Mission End 
	for (FObjective& Objective : MissionDetails.MissionRelatedActions)
	{
		if (Objective.bEvaluateOnMissionEnd)
		{
			Objective.Evaluate();
		}
	}

	UAMS_SubSystem::GetMissionSubSystem()->RecordMissionFinished(this);
	OmMissionEnd(finishState, FailInfo);

	//instead of letting the subsystem take care of removing it i though it would be better to just do it here and make sure that actions are also cleaned if some still rooted
	//DeInitializeMission();
}


void UMissionObject::CountTime(float deltaTime)
{
	if (!MissionDetails.bHasTimer) return;

	TimeCounter = TimeCounter + deltaTime;

	if (TimeCounter >= MissionDetails.MissionTime)
	{
		CurrentState = EFinishState::failed;
		EndMission(CurrentState, FFailInfo());//the default FFailInfo Constructor gives the time out fail info
	}
}

void UMissionObject::DeInitializeMission()
{
	/*for (auto& itr : MissionDetails.MissionRelatedActions)
	{
		if (itr.bIsActivated)
		{
			if (itr.ActivatedAction)
			{
				if(itr.ActivatedAction->IsRooted())
				  itr.ActivatedAction->RemoveFromRoot();
			}
			
		}
		
	}
	
	if(IsRooted())
	RemoveFromRoot();*/
}


void UMissionObject::MissionCheckEnd(TSubclassOf<UActionObject> FinishedAction)
{
	EActionType FinishedObjectiveType = FinishedAction.GetDefaultObject()->ActionType;

	bool bIsRequired = (FinishedObjectiveType == EActionType::required || FinishedObjectiveType == EActionType::InputListener);
	bool bIsBlackListed = FinishedObjectiveType == EActionType::blacklisted;//cuz optional and highscore makes some conflicts so we expelictly check for blacklist

	if (!(bIsRequired || bIsBlackListed)) return;

	int32* CountPtr = (bIsRequired && !bIsBlackListed) ? &RequiredObjectivesCount : &BlackListedObjectivesCount;
	int32 FinishedCount = 0;

	for (auto objective : MissionDetails.MissionRelatedActions)
	{
		/*FinishedObjectiveType != EActionType::optional && FinishedObjectiveType != EActionType::highscore && objective.AffectMissionEnd()*/
		//find all objective of the same type of the currently finished action and see if they all are finished
		if (objective.AsRequired(bIsRequired) && objective.AffectMissionEnd())
		{
			if (objective.bIsFinished)
				FinishedCount++;
		}
	}

	if (FinishedCount == *CountPtr)
	{
		if (FinishedAction.GetDefaultObject()->ActionType == EActionType::required || FinishedObjectiveType == EActionType::InputListener)
		{
			EndMission(EFinishState::succeeded, FFailInfo(EMissionFailReason::notFailed,FinishedAction));
			MissionDetails.bIsMissionFinished = true;
			MissionHasUpdated();
		}
			
	    
		else if (FinishedAction.GetDefaultObject()->ActionType == EActionType::blacklisted)
		{
			EndMission(EFinishState::failed, FFailInfo(FinishedAction));
			MissionHasUpdated();
		}
			
	}
}


#if WITH_EDITOR

void UMissionObject::PostEditChangeProperty(struct FPropertyChangedEvent& changeEvent)
{
	FName PropertyName = changeEvent.Property != nullptr? changeEvent.GetPropertyName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FMissionDetails, MissionTime))
	{
		MissionDetails.OnDetailsChanged();
	}	

}

void UMissionObject::BeginDestroy()
{
	//the mission delete logics that were here is now moved to the subsystem and used asset registry to detect if mission deleted
	//that is better than say it is deleted just when the object is destroyed , cuz when instances got destroyed they act as if the asset
	//is deleted.

	Super::BeginDestroy();
}

TArray<FObjective> UMissionObject::ReturnDefaultObjectives()
{
	return AMS_Types::GenerateDetails(MissionRelatedActions).MissionRelatedActions;
}

#endif

void UMissionObject::SaveGame()
{
	if (MissionSubSystemInstance)
	{
		MissionSubSystemInstance->Internal_MissionSave();
	}
}

