//Advanced Mission System Developed by AIS Studios @2023


#include "MissionObject.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

UMissionObject::UMissionObject()
{
	
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
	LOG_AMS("Mission Is In Progress", 0.0f);
}

TStatId UMissionObject::GetStatId() const
{
	return UObject::GetStatID();
}

bool UMissionObject::IsTickable() const
{
	//only tick if we are not the default object CDO
	return this != GetClass()->GetDefaultObject();
}

UWorld* UMissionObject::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UMissionObject::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	FAssetRegistryTag MissionTag = FAssetRegistryTag(FName("MissionObject"), UMissionObject::StaticClass()->GetName(), FAssetRegistryTag::TT_Alphabetical);
	OutTags.Add(MissionTag);
}

void UMissionObject::InitializeMission(bool bStart)
{
	TMap<TSubclassOf<UActionObject>, int32> InstancesMap;

	if (!bStart) goto FinishInit;

    /*count action instances and increment by action count*/
	for (auto& itr : MissionRelatedActions)
	{
		UActionObject* ActionCDO = itr.GetDefaultObject();

		if (InstancesMap.Contains(itr))
		{
			InstancesMap[itr]+= ActionCDO->LocalActionCount;
		}

		else
		{
			InstancesMap.Add(itr, ActionCDO->LocalActionCount);

			if (ActionCDO->ActionType == EActionType::required)
				RequiredObjectivesCount++;

			if (ActionCDO->ActionType == EActionType::blacklisted)
				BlackListedObjectivesCount++;
		}
	}

	for (auto& kv : InstancesMap)
	{
		MissionDetails.MissionRelatedActions.Emplace(kv.Key, kv.Value);
	}

	FinishInit:
	CurrentState = EFinishState::inProgress;
	OnMissionBegin();
	LOG_AMS("Mission is initilized", 10.0f, FColor::Green);
}

void UMissionObject::EndMission(EFinishState finishState, FFailInfo FailInfo)
{
	CurrentState = finishState;
	UAMS_SubSystem::GetMissionSubSystem()->RecordMissionFinished(this);
	OmMissionEnd(finishState, FailInfo);
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

void UMissionObject::MissionCheckEnd(TSubclassOf<UActionObject> FinishedAction)
{
	EActionType FinishedObjectiveType = FinishedAction.GetDefaultObject()->ActionType;

	int32* CountPtr = FinishedObjectiveType == EActionType::required ? &RequiredObjectivesCount : &BlackListedObjectivesCount;
	int32 FinishedCount = 0;
	for (auto objective : MissionDetails.MissionRelatedActions)
	{
		if (FinishedObjectiveType == FinishedObjectiveType)
		{
			if (objective.bIsFinished)
				FinishedCount++;
		}
	}

	if (FinishedCount == *CountPtr)
	{
		if (FinishedAction.GetDefaultObject()->ActionType == EActionType::required)
			EndMission(EFinishState::succeeded, FFailInfo());
	    
		else
			EndMission(EFinishState::failed, FFailInfo(FinishedAction));
	}
}


void UMissionObject::PostEditChangeProperty(struct FPropertyChangedEvent& changeEvent)
{
	FName PropertyName = changeEvent.Property != nullptr? changeEvent.GetPropertyName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FMissionDetails, MissionTime))
	{
		MissionDetails.OnDetailsChanged();
	}	
}

void UMissionObject::SaveGame()
{
	if (MissionSubSystemInstance)
	{
		MissionSubSystemInstance->Internal_MissionSave();
	}
}

