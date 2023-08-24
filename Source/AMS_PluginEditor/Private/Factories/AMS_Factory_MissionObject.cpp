//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_MissionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include <Kismet2/KismetEditorUtilities.h>
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
		return nullptr;
	}

	else
	{
		UClass* BlueprintClass = nullptr;
		UClass* BlueprintGeneratedClass = nullptr;

		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);
		
		UBlueprint* NewAsset = FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
		
		return NewAsset;
		
	}
}



