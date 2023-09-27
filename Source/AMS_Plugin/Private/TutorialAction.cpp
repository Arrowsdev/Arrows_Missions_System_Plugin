//Advanced Mission System Developed by AIS Studios @2023


#include "TutorialAction.h"
#include "AMS_Plugin/System/AMS_TypesOperations.h"
#include "AMS_Plugin/System/AMS_InputsBindingLib.h"

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

//here we create an input component
void UTutorialAction::OnActionActivated()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// If it doesn't exist create it and bind delegates
		if (!ActionInputComponent)
		{
			static const FName InputCompName("TutorialActionComp");
			ActionInputComponent = NewObject<UInputComponent>(this, InputCompName);
			ActionInputComponent->bBlockInput = false;
			ActionInputComponent->Priority = 5;

			if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
				UAMS_InputsBindingLib::BindInputDelegatesToObject(GetClass(), ActionInputComponent, this);
		}

		PC->PushInputComponent(ActionInputComponent);
		BindTutorialKeys();
	}
}

//delete the input component
void UTutorialAction::OnActionFinished()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		if (ActionInputComponent)
		{
			PC->PopInputComponent(ActionInputComponent);
			ActionInputComponent->DestroyComponent();
			ActionInputComponent = nullptr;
		}
	}
}

void UTutorialAction::BindTutorialKeys()
{
	for (auto& key : TutorialKeys)
	{
		ActionInputComponent->BindKey(key, EInputEvent::IE_Pressed, this, &UTutorialAction::OnTutorialKeyPressed);
	}
}

void UTutorialAction::OnTutorialKeyPressed()
{
	AMS_TypesOperations::TutorialActionSelfPreform(TutorialMission, this->GetClass());
	UE_LOG(LogTemp, Warning, TEXT("Tutorial key pressed"));
}
