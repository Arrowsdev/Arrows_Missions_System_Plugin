//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_ActionObject.h"
#include "Helpers/AMS_Utilites.h"
#include <KismetCompilerModule.h>
#include "AMS_Plugin/System/AMS_CustomBlueprint.h"
#include "AMS_PluginEditor/Public/AMS_PluginEditor.h"

UAMS_Factory_ActionObject::UAMS_Factory_ActionObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActionObject::StaticClass();
}

UObject* UAMS_Factory_ActionObject::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."),
			FText::FromString(ParentClass->GetName())));

		return nullptr;
	}

	else
	{
		UBlueprint* NewAsset = FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			InParent,
			Name,
			BlueprintType,
			UAMS_ActionBlueprint::StaticClass(),
			UAMS_ActionGeneratedBlueprint::StaticClass(),
			CallingContext);

		FAMS_PluginEditor::Get().OnActionAssetCreated(NewAsset->GeneratedClass);
		return NewAsset;
	}
}


bool UAMS_Factory_ActionObject::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Action Object"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FAMsEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk && ChosenClass)
	{
		ParentClass = ChosenClass;
	}
	else
	{
		return false;
	}
	return bPressedOk;
}
