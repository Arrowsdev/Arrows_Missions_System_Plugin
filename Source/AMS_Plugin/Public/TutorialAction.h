//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "ActionObject.h"

#include "TutorialAction.generated.h"

/**
 * this class is a dedecated action object type for input tutorial tasks,
 * this class counts as one even if you have multiple instances of it in the related actions 
 * list, since this should serve as tutorial when the player presses the button 
 * this actoin is considered finished, and it will be treated as required action
 */
UCLASS(HideDropDown, meta=(HideCategories="ActionDetails"))
class AMS_PLUGIN_API UTutorialAction : public UActionObject
{
	GENERATED_BODY()


public:
	UTutorialAction();

	/*the name of this acion that is used for UI preview*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details", meta=(DisplayName="ActionName"))
		FString _ActionName = "type action name here";

	/*inputs that you want the player to press in the tutorial, if one of them is pressed 
	* this action will be finished, it is a list so you can say to the player press this key or that to jump
	* and "this" and "that" are keyboard and gamepad inputs so you dont need to make action for each
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details")
		TArray<FKey> TutorialKeys;

	/*the tutorial mission that require this action*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details")
		TSubclassOf<UMissionObject> TutorialMission;

	UPROPERTY()
	UInputComponent* ActionInputComponent;

	//called by the objective types after the action is activated or finished so this class can use that info to preform the needed logics
	virtual void OnActionActivated() override;
	virtual void OnActionFinished()  override;

	void BindTutorialKeys();
	void OnTutorialKeyPressed();
 
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
