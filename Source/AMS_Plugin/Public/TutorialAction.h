// Fill out your copyright notice in the Description page of Project Settings.

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
UCLASS(meta=(HideCategories="ActionDetails"))
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details", meta = (DisplayName = "ActionName"))
		TArray<FKey> TutorialKeys;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
