// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialAction.h"

UTutorialAction::UTutorialAction()
{
	bCanTick = true;
	ActionType = EActionType::InputListener;

}

#if WITH_EDITOR
void UTutorialAction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTutorialAction, _ActionName))
	{
		ActionName = _ActionName;
	}
}
#endif