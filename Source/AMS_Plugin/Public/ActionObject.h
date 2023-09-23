//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionObject.generated.h"

class UMissionObject;

/**
 * defines the action type
 */
UENUM(BlueprintType)
enum class EActionType : uint8
{
	required      UMETA(DisplayName = "Required Action", ToolTip = "this type means this action has to be done for the mission to progress"),
	optional      UMETA(DisplayName = "Optional Action", ToolTip = "this type means the action is optional and you can provide a bounce for finishing it"),
	blacklisted   UMETA(DisplayName = "BlackListed Action", ToolTip = "this type is the action that should not be done while in this mission, or it will fail"),
	highscore     UMETA(DisplayName = "HighScore Action", ToolTip = "this type means the action will count how many times it was preformed and return it as a score"),
	InputListener UMETA(Hidden)//this is just for internal use to differ the regular actions from input actions
};

/*
* This class is responsable for holding the logics per action and also being the atom for the missions objectives progress counting
* when the mission starts and this action get's activated an instance of it will be created in the world , but it lives for the durtation of the mission 
* and when loading you create new instance so this class is not suitable for holding your custom data  and variables you better if you have some variables 
* to be used with this class create them in the mission class and get them from the owner mission pin in the activation event and also make sure to save 
* this data in the data center by any way you see fits with your game needs , but make sure to not rely much on this class
*/
UCLASS(Blueprintable, BlueprintType, HideDropDown)
class AMS_PLUGIN_API UActionObject : public UObject
{
	GENERATED_BODY()
	/*
	* @Done:
	* action should also pass the mission that it was related to so we can use the action with multiple missions and have different behaviours 
	* this means we need to change how we construct the mission details and the objectives or find a workaround for that 
	*/
public:

	/*the name of this acion that is used for UI preview*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Details")
	    FString ActionName = "type action name here";

	//the type of this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Details")
		EActionType ActionType;

	//if this action can subscribe to it's owner mission tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Details")
		bool bCanTick;

	//how many times this action instance needs to be preformed 
	//if there are multiple instances of this class in the related actions inside the mission
	//this will be multiplied by the number of instances in the related list
	//this one also will be counted as the high socre to meet if the type was highscore
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Details", meta=(DisplayName="Action Count"))
		int32 LocalActionCount = 1;

	//if this action is preformed then the mission will instantly fail.
	UPROPERTY(EditAnywhere, Category = "Action Details", meta=(EditCondition="ActionType == EActionType::blacklisted"))
		bool bInstantFail;

	/*called when ever the action is activated for a mission , this is where you init the needed logics for this action like
	* if this was a kill enemies action then when activated you need to spawn the enemies, and use the provided cout to spawn the number 
	* taking in mind when the mission is loaded after some progress you can use the count to spawn less enemies if the game was checkpoint
	* or use the total to spawn the full number if you want, if the game has saved after doing half of the action count 
	* you need to use just the half to init the action so the player do only the remaining
	*/
	UFUNCTION(BlueprintNativeEvent, Category = ActionEvents)
		void OnActivated(UMissionObject* OnwningMission, int32 count, int32 Total);
	virtual void OnActivated_Implementation(UMissionObject* OnwningMission,int32 count, int32 Total) {/*No Implement*/ }


	/*called when the action is finished*/
	UFUNCTION(BlueprintNativeEvent, Category = ActionEvents)
		void OnFinished(UMissionObject* OnwningMission,int32 count, int32 Total);
	virtual void OnFinished_Implementation(UMissionObject* OnwningMission,int32 count, int32 Total) {/*No Implement*/ }


	//called when the action is preformed, if it is a high score action then it will keep preforming even after the count is done and it will update the
	//highscore objective total count so it will be considered the new highscore to beat
	UFUNCTION(BlueprintNativeEvent, Category = ActionEvents)
		void OnPreformed(UMissionObject* OnwningMission,int32 count, int32 Total);
	virtual void OnPreformed_Implementation(UMissionObject* OnwningMission,int32 count, int32 Total) {/*No Implement*/ }

	//callled when the player reaches a highscore
	//to do : dont know how the rest of the behaviour should be like when we call this , should the objective reset the action count to zero 
	//so it starts to count again or maybe add another member to update the score to meet in the action it self so when ever a highscore is reached
	//we update the score to meet so the player keep counting for new highscore, also should probably save at this point
	UFUNCTION(BlueprintNativeEvent, Category = ActionEvents)
		void OnHighScore(UMissionObject* OnwningMission,int32 count, int32 Total);
	virtual void OnHighScore_Implementation(UMissionObject* OnwningMission,int32 count, int32 Total) {/*No Implement*/ }

	//hooked tick with the owning mission
	UFUNCTION(BlueprintNativeEvent, Category = ActionEvents)
		void ActionTick(float deltaTime);
	virtual void ActionTick_Implementation(float deltaTime) {/*No Implement*/ }

	UWorld* GetWorld() const;

};
