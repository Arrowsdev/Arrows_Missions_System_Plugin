// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AMS_Plugin/Public/ActionObject.h"
#include "AMS_Types.generated.h"


//debugging macro for logging the function that is using the (PringLog) Function, mainly used for logging strings 
//you need to format your data first and pass it as a string
#define LOG_AMS(string,...)\
 PrintLog(FString::Printf(TEXT("[ %s Function in line - %d - ] : %s"),\
          *FString(__func__),__LINE__, *FString(string)),__VA_ARGS__);

 
#define INIT_START true
#define INIT_LOAD false

class UActionObject;
class UMissionObject;


inline void PrintLog(FString log, float duration=0, FColor color = FColor::Blue)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, color, log);
	}
}

//ENUMURATION: 

//enum to represent mission finish state
UENUM(BlueprintType)
enum class EFinishState : uint8
{
	inProgress UMETA(DisplayName = "In Progress"),
	paused     UMETA(DisplayName = "Paused"),
	succeeded  UMETA(DisplayName="Succeeded"),
	failed     UMETA(DisplayName = "Failed"),
	canceled   UMETA(DisplayName = "Canceled")
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

	FObjective(TSubclassOf<UActionObject> Action, int32 count)
		:ActionClass(Action), TotalCount(count)
	{
		//we dont want to create new object instead using the class CDO to call our logic in BP 
		ActivatedAction = ActionClass.GetDefaultObject();
		bIsFinished = false;
		ActionCount = 0;
	}


	bool Preform()// true means finished false means not finished or not required to finish
	{
		if (!bIsFinished)
		{
			ActionCount++;
			ActivatedAction->OnPreformed(ActionCount, TotalCount);

		    if (ActionCount >= TotalCount)
			{
				if (ActivatedAction->ActionType == EActionType::highscore)
				{
					TotalCount = ActionCount;
					//should save here

					return false;
				}

				bIsFinished = true;
				ActivatedAction->OnFinished(ActionCount, TotalCount);

				return true;//only return true if the objective is finished so we can try to check if all others are also finished
			}

		}

		else
		PrintLog("Objective is finished , preform is ignored !", 10.0f, FColor::Red);

		return bIsFinished;
	}

	//gets the status in : ActionName [ 1 / 5 ]
	FString GetObjectibeStatus(EStatusGetterType getterType)
	{
		switch (getterType)
		{
		    case EStatusGetterType::current:
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActivatedAction->ActionName, ActionCount);
			}
			case EStatusGetterType::total :
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActivatedAction->ActionName, TotalCount);
			}
			case EStatusGetterType::current_total:
			{
				return FString::Printf(TEXT("%s [ %d / %d ]"), *ActivatedAction->ActionName, ActionCount, TotalCount);
			}
			case EStatusGetterType::total_current:
			{
				return FString::Printf(TEXT("%s [ %d / %d ]"), *ActivatedAction->ActionName, TotalCount, ActionCount);
			}
		    default:
			{
				return FString::Printf(TEXT("%s [ %d ]"), *ActivatedAction->ActionName, ActionCount);
			}
		}
		
	}


	bool operator==(TSubclassOf<UActionObject> action)
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
		return FMath::CeilToFloat(ActionCount) / FMath::CeilToFloat(TotalCount);
	}
};

USTRUCT(BlueprintType)
struct FMissionDetails
{
	GENERATED_USTRUCT_BODY()


		FMissionDetails() {}

	//the name of the mission used to preview in ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta=(DisplayName="Mission Name"))
		FString MissionName = "Default Mission Name";

	//the describtion of the mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionDetails", meta = (DisplayName = "Mission Describtion"))
		FString MissionDescribtion = "Default Mission Describtion";

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
				if (objective == EActionType::required)
				{
					percent += objective.GetObjectiveProgress();
				}
			}
			MissionProgress = percent;
			return percent;
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
		RequiredCount(otherRecord.RequiredCount),BlackListedCount(otherRecord.BlackListedCount)
	{
		
	}

	UPROPERTY(BlueprintReadWrite, Category = "Mission Records", meta = (DisplayName = "Finished Mission"))
		TSubclassOf<UMissionObject> MissionClass;

	UPROPERTY(BlueprintReadWrite, Category = "Mission Records", meta = (DisplayName = "Mission Details"))
		FMissionDetails MissionDetails;


	//used to save the count of the required taskes from the mission
	UPROPERTY()
		int32 RequiredCount;

	UPROPERTY()
		int32 BlackListedCount;

	FRecordEntry(TSubclassOf<UMissionObject> Mission, FMissionDetails details) 
		: MissionClass(Mission), MissionDetails(details)
	{
	} 

	FRecordEntry(TSubclassOf<UMissionObject> Mission, FMissionDetails details, int32 required, int32 blacklisted)
		: MissionClass(Mission), MissionDetails(details)
	{
		RequiredCount = required;
		BlackListedCount = blacklisted;
	}
};

/**
 *
 */
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
			outDetails.MissionRelatedActions.Emplace(kv.Key, kv.Value);
		}

		return outDetails;
	}
};
