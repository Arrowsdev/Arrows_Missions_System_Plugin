// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AMS_Plugin/Public/ActionObject.h"
#include "AMS_LOG.h"
#include "AMS_TypesOperations.h"

#include "AMS_Types.generated.h"


//debugging macro for logging the function that is using the (PringLog) Function, mainly used for logging strings 
//you need to format your data first and pass it as a string
#define LOG_AMS(string,...)\
 PrintLog(FString::Printf(TEXT("[ %s Function in line - %d - ] : %s"),\
          *FString(__func__),__LINE__, *FString(string)),__VA_ARGS__);

 
#define INIT_START true
#define INIT_LOAD false

class UActionObject;
class UMissionObject;//using AMS_TypesOperation to workaround this forwarded class type cuz i cant use it cuz it is incomplete and adding the header will cause
//dependacy loop


inline void PrintLog(FString log, float duration = 0, FColor color = FColor::Blue)
{
	if (GEngine)
	{
		if(AMS_LOG::IsOnScreenDebugIsEnabled())
		   GEngine->AddOnScreenDebugMessage(-1, duration, color, log);

		UE_LOG(MissionSystem, Warning, TEXT("%s"), *log);//so we log to the consol also
	}
}

//ENUMURATION: 

//enum to represent mission finish state
UENUM(BlueprintType)
enum class EFinishState : uint8
{
	inProgress UMETA(DisplayName = "In Progress"),
	paused     UMETA(DisplayName = "Paused"),
	succeeded  UMETA(DisplayName = "Succeeded"),
	failed     UMETA(DisplayName = "Failed"),
	canceled   UMETA(DisplayName = "Canceled"),
	notPlayed UMETA(DisplayName = "not played", Hidden)//this for missions that were collected by the subsystem for ui display we dont want to have this option visible
	//anywhere in the blueprint code , this just to print an accurate text if the mission is not even started or canceled this is the state that it is in
};

//reasons for mission to fail
UENUM(BlueprintType)
enum class EMissionFailReason : uint8
{
	timeOut        UMETA(DisplayName = "Time Out"),
	PlayerAction   UMETA(DisplayName = "Player Action"),
	canceled       UMETA(DisplayName = "Canceled")
};

UENUM(BlueprintType)
enum class ESaveMissionType : uint8
{
	none     UMETA(DisplayName = "Dont Save", ToolTip = "the game will never save"),
	Post     UMETA(DisplayName = "After Mission Completion", ToolTip = "the game will save after the player finishes a mission"),
	Always   UMETA(DisplayName = "After Any Task Preformed",  ToolTip = "the game will save per task completion")
};

UENUM(BlueprintType)
enum class EStatusGetterType : uint8
{
	current       UMETA(DisplayName = "Current"),
	total         UMETA(DisplayName = "Total"),
	current_total UMETA(DisplayName = "Current / Total"),
	total_current UMETA(DisplayName = "Total / Current")
};

//defines how we restart mission, if from checkpoint or from mission start
UENUM(BlueprintType)
enum class ERestartType : uint8
{
	fromCheckPoint,
	fromStart
};

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	main   UMETA(DisplayName = "Main Mission", ToolTip = "needed for the game to finish"),
	side   UMETA(DisplayName = "Side Mission", ToolTip = "not needed for the game to finish \n but considered in the full game progress")
};

//STRUCTURES:

//this is the representation of the action after activation
//it holds the count of the action instances in the array and 
//the state of completion of it
USTRUCT(BlueprintType)
struct FObjective
{

	GENERATED_BODY()

	FObjective() {};

	UPROPERTY(BlueprintReadWrite, Category = "Objective")
	UActionObject* ActivatedAction;

	//to prevent temp values form being optimized away by the garbage collector while saving game
	UPROPERTY()
	TSubclassOf<UActionObject> ActionClass;

	UPROPERTY(BlueprintReadWrite, Category="Objective")
	int32 ActionCount;

	//this is the total instances count of the action that needs to be preformed
	//so the objective can be considered finished. it also works as a high score
	UPROPERTY(BlueprintReadWrite, Category = "Objective")
	int32 TotalCount;

	UPROPERTY()
	bool bIsFinished;//will be true if preformed X times, while X is the Action count

	UPROPERTY()
	UMissionObject* OwningMission;

	UPROPERTY()
	EActionType ActionType;

	UPROPERTY()
	FString ActionName;

	//default constructor , that is used when creating objective struct 
	FObjective(TSubclassOf<UActionObject> Action, int32 count, UMissionObject* _owningMission)
		:ActionClass(Action), TotalCount(count),OwningMission(_owningMission)
	{
		//we dont want to create new object instead using the class CDO to call our logic in BP 
		//thinking of making instances instead of using the cdo so we can use custom user variables and they can differ and no worries if multiple missions had the same
		//action,
		ActionType = Cast<UActionObject>(ActionClass->GetDefaultObject())->ActionType;
		ActionName = Cast<UActionObject>(ActionClass->GetDefaultObject())->ActionName;
		bIsFinished = false;
		ActionCount = 0;
	}


	bool Preform()// true means finished false means not finished or not required to finish
	{
		if (!bIsFinished)
		{
			ActionCount++;
			ActivatedAction->OnPreformed(OwningMission,ActionCount, TotalCount);

		    if (ActionCount >= TotalCount)
			{
				if (ActivatedAction->ActionType == EActionType::highscore)
				{
					TotalCount = ActionCount;

					ActivatedAction->OnHighScore(OwningMission,ActionCount, TotalCount);
					//should save here

					return false;
				}

				bIsFinished = true;
				ActivatedAction->OnFinished(OwningMission,ActionCount, TotalCount);
				AMS_TypesOperations::InvokeOnTaskFinished(OwningMission, ActionClass, ActionCount);

				return true;//only return true if the objective is finished so we can try to check if all others are also finished
			}

		}

		else
		PrintLog("Objective is finished , preform is ignored !", 10.0f, FColor::Red);

		return bIsFinished;
	}

	//called from the details to activate the action assossiated with this objective
	void Activate()
	{
		//when activating we replace the cdo with new object instance
		ActivatedAction = AMS_TypesOperations::NewActionObject(OwningMission, ActionClass);

		//this so we dont need to rely on it's instance when we load the objective in the finished list
		/*ActionType = ActivatedAction->ActionType;
		ActionName = ActivatedAction->ActionName;*/
	

		ActivatedAction->OnActivated(OwningMission,ActionCount, TotalCount);
		AMS_TypesOperations::InvokeOnTaskActivated(OwningMission, ActionClass, ActionCount, TotalCount);

		if (ActivatedAction->bCanTick)
		{
			AMS_TypesOperations::SubscribeToMissionTick(OwningMission, ActivatedAction);
		}
	}

	void Tick(float deltaTime)//deprecated
	{
		if (ActivatedAction)
		{
			ActivatedAction->ActionTick(deltaTime);
		}
	}
	//gets the status in : ActionName [ 1 / 5 ]
	FString GetObjectibeStatus(EStatusGetterType getterType, FString& ObjectiveType)
	{
		//remove the enum class namespace and leave the value
		ObjectiveType = UEnum::GetValueAsString(ActionType);
		FString SFrom = TEXT("EActionType::");
		FString STo = TEXT("");
		const TCHAR* From = *SFrom;
		const TCHAR* To = *STo;
		ObjectiveType = ObjectiveType.Replace(From, To);

		switch (getterType)
		{
		    case EStatusGetterType::current:
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActionName, ActionCount);
			}
			case EStatusGetterType::total :
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActionName, TotalCount);
			}
			case EStatusGetterType::current_total:
			{
				return FString::Printf(TEXT("%s [ %d / %d ]"), *ActionName, ActionCount, TotalCount);
			}
			case EStatusGetterType::total_current:
			{
				return FString::Printf(TEXT("%s [ %d / %d ]"), *ActionName, TotalCount, ActionCount);
			}
		    default:
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActionName, ActionCount);
			}
		}
		
	}


	bool operator==(const TSubclassOf<UActionObject>& action)
	{
		return ActionClass == action;
	}

	bool operator==(EActionType actionType)
	{
		return ActionClass.GetDefaultObject()->ActionType == actionType;
	}

	//gets the percentage of compeletion for the objective
	float GetObjectiveProgress()
	{
		return static_cast<float>(ActionCount) / static_cast<float>(TotalCount);
	}

	bool HasOwner()
	{
		return OwningMission != nullptr;
	}

	bool AffectMissionEnd()
	{
		return (ActionClass.GetDefaultObject()->ActionType == EActionType::required) || (ActionClass.GetDefaultObject()->ActionType == EActionType::blacklisted);
	}
};

/*
* holds the information for a mission instance 
*/
USTRUCT(BlueprintType)
struct FMissionDetails
{
	GENERATED_USTRUCT_BODY()


	FMissionDetails() { CurrentState = EFinishState::notPlayed; }

	//the name of the mission used to preview in ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta=(DisplayName="Mission Name"))
		FString MissionName = "Default Mission Name";

	//the describtion of the mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Mission Describtion"))
		FString MissionDescribtion = "Default Mission Describtion";

	//if the mission is main or side mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Mission Type"))
		EMissionType MissionType;

	//if the mission have a count down for it's end or not
	UPROPERTY(EditAnywhere, Category = "MissionDetails", meta = (DisplayName = "Has Timer?"))
		bool bHasTimer;

	//if the mission has a timer this is the amount of time it has before it timeout 
	UPROPERTY(EditAnywhere, Category = "MissionDetails", meta = (DisplayName = "Mission Time", EditCondition = "bHasTimer"))
		float MissionTime;

	//hidden member used just to store the default time the user set for the Mission Time
	//so we can use it to restart the mission from a record with proper default values
	UPROPERTY()
	float DefaultMissionTime;

	UPROPERTY()
	bool bIsMissionFinished;

	UPROPERTY()
	int32 RequiredCount;

	UPROPERTY()
	int32 OptionalCount;

	//saving the state here to avoid the loss of the actual value from the mission when saving , this is a work around for a bug
	UPROPERTY(BlueprintReadWrite, Category = "MissionDetails")
	EFinishState CurrentState;

	//used for when recording the mission we need a place to put the calculated progress
	UPROPERTY(BlueprintReadWrite, Category = "MissionDetails")
	float MissionProgress;

	//how we count , is it count down or otherwise
	UPROPERTY(EditAnywhere, Category = "MissionDetails", meta = (DisplayName = "Is Count Down?", EditCondition = "bHasTimer"))
		bool bIsCountDown = true;

	//the main objectives to preform to pass the mission
	UPROPERTY(BlueprintReadWrite, Category = "MissionDetails")
	    TArray<FObjective> MissionRelatedActions;


		//find the objective for specific action to preform
		FObjective& GetActionRelatedObjective(TSubclassOf<UActionObject> action)
		{
			static FObjective defaultObjective = FObjective();

			for (auto& itr : MissionRelatedActions)
			{
				if (itr == action) return itr;
			}

			return defaultObjective;
		}

		//when ever the user sets the mission time value we set the default value
		//this for when we save the mission record we want to knwo the default time and the time 
		//where the mission ended
		void OnDetailsChanged()
		{
			DefaultMissionTime = MissionTime;
		}

		float GetMissionCompeletion()
		{
			float percent = 0.0f;

			for (FObjective& objective : MissionRelatedActions)
			{
				if (objective == EActionType::required || objective == EActionType::optional)
				{
					percent += objective.GetObjectiveProgress();
				}
			}
			MissionProgress = percent / (RequiredCount + OptionalCount);//we need to re think about this, cuz optional tasks should be counted in the progress, but not to affect the is finished check

			return MissionProgress;
		}

		void ActivateActions(UMissionObject* Owner)
		{
			CurrentState = EFinishState::inProgress;

			for (auto& action : MissionRelatedActions)
			{
				if (!action.HasOwner())
					action.OwningMission = Owner;

				action.Activate();
			}
		}

		//called from the mission init function 
	    void InitOptionalCout()
		{
			OptionalCount = 0;
			for (auto& itr : MissionRelatedActions)
			{
				if (itr.ActionType == EActionType::optional)
				{
					OptionalCount++;
				}
			}
		}
};

USTRUCT(BlueprintType)
struct FFailInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Mission Logics", meta = (DisplayName = "Fail Reason"))
		EMissionFailReason FailReason;

	UPROPERTY(BlueprintReadWrite, Category = "Mission Logics", meta = (DisplayName = "Caused Action"))
		TSubclassOf<UActionObject> CausedAction;

	//by default we consider the fail reason is time out
	FFailInfo()
	{
		FailReason = EMissionFailReason::timeOut;
	}

	//player action as if the player killed important NPC for mission progression, so it fails
	FFailInfo(TSubclassOf<UActionObject> Action) : CausedAction(Action)
	{
		FailReason = EMissionFailReason::PlayerAction;
	}

	//if canceled
	FFailInfo(EMissionFailReason Reason) : FailReason(Reason)
	{

	}

};

//used to record missions finish states in the subsystem
USTRUCT(BlueprintType)
struct FRecordEntry
{
	GENERATED_BODY()

	FRecordEntry() {};

	FRecordEntry(const FRecordEntry& otherRecord):
		MissionClass(otherRecord.MissionClass), MissionDetails(otherRecord.MissionDetails),
		MissionState(otherRecord.MissionState), RequiredCount(otherRecord.RequiredCount),
		BlackListedCount(otherRecord.BlackListedCount)
	{
		
	}

	UPROPERTY(BlueprintReadWrite, Category = "Mission Records", meta = (DisplayName = "Finished Mission"))
		TSubclassOf<UMissionObject> MissionClass;

	UPROPERTY(BlueprintReadWrite, Category = "Mission Records", meta = (DisplayName = "Mission Details"))
		FMissionDetails MissionDetails;

	UPROPERTY(BlueprintReadWrite, Category = "Mission Records", meta = (DisplayName = "Mission State"))
		EFinishState MissionState;

	//used to save the count of the required taskes from the mission
	UPROPERTY()
		int32 RequiredCount;

	UPROPERTY()
		int32 BlackListedCount;

	FRecordEntry(TSubclassOf<UMissionObject> Mission, FMissionDetails details) 
		: MissionClass(Mission), MissionDetails(details)
	{
	} 

	FRecordEntry(TSubclassOf<UMissionObject> Mission, FMissionDetails details, EFinishState missionState, int32 required, int32 blacklisted)
		: MissionClass(Mission), MissionDetails(details)
	{
		RequiredCount = required;
		BlackListedCount = blacklisted;
	}

	//get if a mission is finsihed or not and the progress of it 
	bool IsFinished(float& completion)
	{
		completion = MissionDetails.GetMissionCompeletion();
		return MissionDetails.bIsMissionFinished;
	}

	bool operator==(const FRecordEntry& other)
	{
		return MissionClass == other.MissionClass;
	}

	bool operator==(const TSubclassOf<UMissionObject> mission)
	{
		return MissionClass == mission;
	}
};


class AMS_PLUGIN_API AMS_Types
{

public:
	AMS_Types();
	~AMS_Types();

	//generate temp details without the need for a hosting mission, so we can make calculations depending on properties of classes
	//and we dont need to init any mission to get the details that it will have if it was started
	static FMissionDetails GenerateDetails(TArray<TSubclassOf<UActionObject>>& Actions)
	{
		TMap<TSubclassOf<UActionObject>, int32> InstancesMap;
		FMissionDetails outDetails;
		UMissionObject* hostMission = nullptr;
		for (auto& itr : Actions)
		{
			UActionObject* ActionCDO = itr.GetDefaultObject();

			if (InstancesMap.Contains(itr))
			{
				InstancesMap[itr] += ActionCDO->LocalActionCount;
			}
			else
			{
				InstancesMap.Add(itr, ActionCDO->LocalActionCount);
			}
		}

		for (auto& kv : InstancesMap)
		{
			outDetails.MissionRelatedActions.Emplace(kv.Key, kv.Value,hostMission);
		}

		return outDetails;
	}

};


