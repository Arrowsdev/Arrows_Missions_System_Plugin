//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "AMS_Plugin/System/AMS_Types.h"

#include "Kismet/KismetMathLibrary.h"

#include "MissionObject.generated.h"


/**
 * This class is responsible for missions logics implementation
 */
class AMS_SubSystem;

UCLASS(Blueprintable, BlueprintType)
class AMS_PLUGIN_API UMissionObject : public UObject , public FTickableGameObject
{
	GENERATED_BODY()

	UMissionObject();
	~UMissionObject();

    /*Tickable object interface*/
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	/*Tickable Object Interface*/

	//World Context for static functions in blueprints to be called normally 
	UWorld* GetWorld() const;


	//## Mission Object Interface ## //
public:  

    //## Core Mission Events ##

	/*Called When The Mission Is Started And Initilized*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OnMissionBegin();
	virtual void OnMissionBegin_Implementation() {/*No Implement*/ }

	/*Called When The Mission Is Ended And It Spits the finish results*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OmMissionEnd(EFinishState finishState, FFailInfo FailInfo);
	virtual void OmMissionEnd_Implementation(EFinishState finishState, FFailInfo FailInfo) {/*No Implement*/ }

	/*called every tick while the mission is active*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void MissionTick(float deltaTime);
	virtual void MissionTick_Implementation(float deltaTime) {/*No Implement*/ }

	/*called when ever a task is activated, the activation for task is meaning they are now ready
	* to consider the player action , for example we have a missino of two tasks one is go to some place and the second is 
	* destroy somthing there , we want the player to first go there before we tell him about what to do this 
	* is the use of task activation, to get that behaviour we just delay the activation for the second task.
	* you can set this in the mission settings (task activation settings)
	*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OnTaskActivated();
	virtual void OnTaskActivated_Implementation() {/*No Implement*/ }

	/*called when ever a task is finished*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OnTaskFinished();
	virtual void OnTaskFinished_Implementation() {/*No Implement*/ }

	//Mission Configuration
	UPROPERTY(EditAnywhere, Category = Settings)
		FMissionDetails MissionDetails;

	//list of all actions related to this mission, put here main objectives
	//and blacklisted actions and bounce and every action related to it
	UPROPERTY(EditAnywhere, Category = Settings)
		TArray<TSubclassOf<UActionObject>> MissionRelatedActions;

	/*should we automatically restart if mission is faild or we wait for restart mission function call*/
	UPROPERTY(EditAnywhere, Category = Settings, meta=(DisplayName="Auto Restart?"))
		bool bAutoRestart;

	/*if this value is set then directly when this mission is passed it will open the next mission*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (DisplayName = "Next Mission"))
		TSubclassOf<UMissionObject> NextMission;

	/*the level in which the mission is taking place , if this is not set then the mission will play in what even level the character is in currently*/
	UPROPERTY(EditAnywhere, Category = Settings, meta = (DisplayName = "Mission Level"))
		FName MissionLevel;

	// Mission API

	/*Gets The Current Time Of the Mission
	* @param formatedTime gives the time in minutes : seconds fasion
	*/
	UFUNCTION(BlueprintPure, Category="Mission System", meta=(ReturnDisplayName="Current Time"))
		FORCEINLINE float GetMissionTime(FText& formatedTime, float& defaultTime)
    {
		//seconds counter
		int32 TotalSeconds = MissionDetails.bIsCountDown? MissionDetails.MissionTime - TimeCounter : TimeCounter;

		//logics for formated time
		double seconds;
		int32 Minutes = UKismetMathLibrary::FMod((FMath::CeilToDouble(TotalSeconds)), FMath::CeilToDouble(60.0f), seconds);

		FString AdditionalText = CurrentState != EFinishState::paused ? "" : " [ Paused ]";
		formatedTime = FText::FromString(FString::Printf(TEXT("[ %d : %d ]%s"), Minutes, FMath::RoundToInt(seconds), *AdditionalText));

		defaultTime = MissionDetails.DefaultMissionTime;
	    return TotalSeconds;
    }

	/*return the objective as : objective[completion], while the completion depends on the format */
	UFUNCTION(BlueprintPure, Category = "Mission System", meta = (ReturnDisplayName = "Objetive Status"))
		FORCEINLINE FString GetObjectiveAsString(FObjective objective, EStatusGetterType format)
	{
		return objective.GetObjectibeStatus(format);
	}

	/*returns the generated objectives from the actions provided in the class details */
	UFUNCTION(BlueprintPure, Category = "Mission System", meta = (ReturnDisplayName = "Objectives List"))
		FORCEINLINE TArray<FObjective> GetObjectiveList()
	{
		return MissionDetails.MissionRelatedActions;
	}

	/*used to debug the number of generated objectives for provided actions*/
	UFUNCTION(BlueprintPure, Category = "Mission System", meta = (ReturnDisplayName = "Mission State"))
		FORCEINLINE	EFinishState GetObjectiveCount()
	{
		return CurrentState;
	}

	UFUNCTION(BlueprintPure, Category = "Mission System | Debugging", meta = (ReturnDisplayName = "Mission Details"))
		FORCEINLINE	FMissionDetails GetMissionDetails(float& DefaultValue)
	{
		DefaultValue = MissionDetails.DefaultMissionTime;
		return MissionDetails;
	}

	//used to add time to the current mission time
	UFUNCTION(BlueprintCallable, Category = "Mission System")
		FORCEINLINE void AddMissionTime(float amount, bool countDown)
	{
		MissionDetails.MissionTime = MissionDetails.MissionTime + amount;
		MissionDetails.bIsCountDown = countDown;
	}


// NO EXPOSE:
// 
	//Core Properties:
	float TimeCounter;
	int32 RequiredObjectivesCount;
	int32 BlackListedObjectivesCount;

	EFinishState CurrentState;

	//Core Functions
	// 
	//initilize objectives and calls mission begin
	void InitializeMission();
	void EndMission(EFinishState finishState, FFailInfo FailInfo);
	void CountTime(float deltaTime);

	//called from the subsystem when the preformed action is finished so we need to check if all other actions from the same type are also finished
	void MissionCheckEnd(TSubclassOf<UActionObject> FinishedAction);

	//when the user changes some values in the details pannel we need to refelect this to the details structs for it to 
	//preform the needed logics , in this case we want the private default time to match the user set value so we can
	//use it to restart the mission from the juernal records
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	
};
