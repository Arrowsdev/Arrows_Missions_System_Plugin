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
	canceled       UMETA(DisplayName = "Canceled"),
	notFailed      UMETA(DisplayName = "Didn't Fail")
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

//defines how we activate the tasks , if they should be in order like activate the first one 
//and wait for it to finish then activate the next one 
UENUM(BlueprintType)
enum class ETasksOrderPolicy : uint8
{
	noOrder   UMETA(DisplayName = "Not Ordered Tasks", ToolTip = "all actions are activated on start\n and they can be done in any order"),
	order     UMETA(DisplayName = "Ordered Taks", ToolTip = "activate actions one by one after each action finish")
};

//STRUCTURES:

//this is the representation of the action after activation
//it holds the count of the action instances in the array and 
//the state of completion of it
USTRUCT(BlueprintType)
struct FObjective
{

	GENERATED_BODY()

	FObjective()
	{
		ActivatedAction = nullptr;
		ActionClass = nullptr;
		ActionCount = 0;
		TotalCount = 0;
		bIsFinished = false;
		bIsActivated = false;
		OwningMission = nullptr;
		ActionType = EActionType::NotDefined;
	};

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
	bool bIsActivated;

	UPROPERTY()
	UMissionObject* OwningMission;

	UPROPERTY()
	EActionType ActionType;//should expose ?

	UPROPERTY()
	FString ActionName;

	UPROPERTY()
	int32 ObjectiveID;

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
		ObjectiveID = INDEX_NONE;
	}

	//dedecated constructor to create objective struct that is not gameplay related , it is aimed to fix the crash when we have a full game
	//missions record we dont have activated action so we pass the cdo instead.
    explicit FObjective(TSubclassOf<UActionObject> Action,UActionObject* AsActivatede, int32 count, UMissionObject* _owningMission)
		:ActivatedAction(AsActivatede), ActionClass(Action), TotalCount(count), OwningMission(_owningMission)
	{
		ActionType = Cast<UActionObject>(ActionClass->GetDefaultObject())->ActionType;
		ActionName = Cast<UActionObject>(ActionClass->GetDefaultObject())->ActionName;
		bIsFinished = false;
		ActionCount = 0;
		ObjectiveID = INDEX_NONE;
	}

	bool Preform()// true means finished false means not finished or not required to finish
	{
		if (!bIsFinished && bIsActivated && ActivatedAction)
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
				ActivatedAction->OnActionFinished();
				AMS_TypesOperations::InvokeOnTaskFinished(OwningMission, ActionClass, ActionCount);

				if (HasOwner())
				{
					AMS_TypesOperations::InvokeActivateNext(ObjectiveID+1, OwningMission);
				}

				AMS_TypesOperations::RemoveActionFromRoot(ActivatedAction);

				return true;//only return true if the objective is finished so we can try to check if all others are also finished
			}

		}

		else
		PrintLog("Objective is finished , preform is ignored !", 10.0f, FColor::Red);

		return bIsFinished;
	}

	//called from the details to activate the action assossiated with this objective
	void Activate(int32 ID)
	{
		bIsActivated = true;
		ObjectiveID = ID;

		//when activating we replace the cdo with new object instance
		ActivatedAction = AMS_TypesOperations::NewActionObject(OwningMission, ActionClass);

		//we dont add to root now , not for action or missions i want to test something so we can do this in much cleaner way
		//AMS_TypesOperations::AddActionToRoot(ActivatedAction);

		//this so we dont need to rely on it's instance when we load the objective in the finished list
		/*ActionType = ActivatedAction->ActionType;
		ActionName = ActivatedAction->ActionName;*/
	

		ActivatedAction->OnActivated(OwningMission,ActionCount, TotalCount);
		ActivatedAction->OnActionActivated();
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
				FString Activ = bIsActivated ? "Active" : "Not Active";
				return FString::Printf(TEXT("%s [ %d / %d ] %s with ID : %d"), *ActionName, ActionCount, TotalCount,*Activ, ObjectiveID);
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
		return (ActionClass.GetDefaultObject()->ActionType == EActionType::required)
			   || (ActionClass.GetDefaultObject()->ActionType == EActionType::blacklisted
			   || ActionClass.GetDefaultObject()->ActionType == EActionType::InputListener);
	}
};

/*
* holds the information for a mission instance 
*/
USTRUCT(BlueprintType)
struct FMissionDetails
{
	GENERATED_USTRUCT_BODY()


		FMissionDetails()
	    { 
		   CurrentState = EFinishState::notPlayed;
		   MissionType = EMissionType::main;
		   OrderPolicy = ETasksOrderPolicy::order;
		   bHasTimer = false;
		   MissionTime = 0.0f;
		   DefaultMissionTime = 0.0f;
		   RequiredCount = 0;
		   OptionalCount = 0;
		   MissionProgress = 0.0f;
		   bHasCustomStartPostion = false;
		   bIsMissionFinished = false;
	    }

	//the name of the mission used to preview in ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta=(DisplayName="Mission Name"))
		FString MissionName = "Default Mission Name";

	//the describtion of the mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Mission Describtion"))
		FString MissionDescribtion = "Default Mission Describtion";

	//if the mission is main or side mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Mission Type"))
		EMissionType MissionType;

	//define how the mission tasks should be, if order of execution is needed choose the ordered policy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Tasks Order Policy"))
		ETasksOrderPolicy OrderPolicy;

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

	//if the mission has a custom start location, if so then before we init mission data we transfair the player to that location
	UPROPERTY(EditAnywhere, Category = "MissionDetails")
		bool bHasCustomStartPostion;

	//the transform used for the mission when started fresh, other wise we use the start transform from the subsystem since we may have many active missions 
	//the subsystem record the latest used for mission start or location when check point is reached
	//position for load game and restart game so it wont get confused
	UPROPERTY(EditAnywhere, Category = "MissionDetails", meta = (EditCondition = "bHasCustomStartPostion"))
		FTransform StartTransform;

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
			int32 ID = 0;
			if (OrderPolicy == ETasksOrderPolicy::noOrder)
			{
				for (auto& action : MissionRelatedActions)
				{
					if (!action.HasOwner())
						action.OwningMission = Owner;

					action.Activate(ID);
					ID++;
				}
			}
			else if (OrderPolicy == ETasksOrderPolicy::order)
			{
				//if the mission was loaded then we need to make sure we walk through all objectives and 
				//see the first inactive one and activate it and not just activate the first  one since maybe we are loading 
				//after the player has finished the first one
				for (auto& action : MissionRelatedActions)
				{
					if (!action.bIsActivated)
					{
						ActivateNext(ID, Owner);
						break;
					}

					else if (!action.bIsFinished && action.bIsActivated)
					{
						//if loaded the mission from save then it is surely wont have an owner mission since the owner would have been cleaned when closed
						//so we give it a new instance of the owner , this is the one that is started from load
						if (!action.HasOwner())
							action.OwningMission = Owner;

						//if it was activated before closing and loaded we want to just activate it again so it has a valid action pointer
						action.Activate(ID);
					}

					ID++;
				}
				
			}
		}

		//activate one action after another
		void ActivateNext(int32 NextID, UMissionObject* Owner)
		{
			//cuz we are calling this from the action finish logics we want to make sure if the policy was no order then this function
			//should not be accounted 
			if (OrderPolicy != ETasksOrderPolicy::order) return;

			if (MissionRelatedActions.IsValidIndex(NextID))
			{
				auto& action = MissionRelatedActions[NextID];

				if (!action.HasOwner())
					action.OwningMission = Owner;

				action.Activate(NextID);
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

	//constructor for when the mission is passed we need valid way to represent this case , so i added the new fail reason "notFail"
explicit FFailInfo(EMissionFailReason Reason, TSubclassOf<UActionObject> Action):
	     FailReason(Reason), CausedAction(Action)
	{

	}
};

//used to record missions finish states in the subsystem
USTRUCT(BlueprintType)
struct FRecordEntry
{
	GENERATED_BODY()

	FRecordEntry()
	{
		MissionState = EFinishState::notPlayed;
		MissionClass = nullptr;
		RequiredCount = 0;
		BlackListedCount = 0;
	};

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
			//using the new constructor 
			outDetails.MissionRelatedActions.Emplace(kv.Key, kv.Key.GetDefaultObject(), kv.Value,hostMission);
		}

		return outDetails;
	}

};


