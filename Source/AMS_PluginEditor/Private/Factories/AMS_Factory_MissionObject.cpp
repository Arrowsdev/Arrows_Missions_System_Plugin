//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_MissionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include <Kismet2/KismetEditorUtilities.h>
#include "Helpers/AMS_Utilites.h"
#include <KismetCompilerModule.h>




UAMS_Factory_MissionObject::UAMS_Factory_MissionObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMissionObject::StaticClass();

	ParentClass = SupportedClass;
}

UObject* UAMS_Factory_MissionObject::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		//open dialog when cant create new mission object since we are now have the ability to inhert from bp classes so the 
		//user can have his own  parent mission with custom logics and make childs out of it
		FMessageDialog::Open(EAppMsgType::Ok,
		FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."),
		FText::FromString(ParentClass->GetName())));

		return nullptr;
	}

	else
	{
		UClass* BlueprintClass = nullptr;
		UClass* BlueprintGeneratedClass = nullptr;
		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);
		
		UBlueprint* NewAsset = FKismetEditorUtilities::CreateBlueprint(
			    ParentClass,
			    InParent,
			    Name,
			   BlueprintType,
			   BlueprintClass,
			   BlueprintGeneratedClass,
			   CallingContext);
	
		return NewAsset;
	}
}

bool UAMS_Factory_MissionObject::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mission Object"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FAMsEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}



