//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_DataCenter.h"
#include "AMS_Plugin/System/AMS_DataCenter.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AMS_Plugin/System/AMS_CustomBlueprint.h"

UAMS_Factory_DataCenter::UAMS_Factory_DataCenter(const FObjectInitializer& ObjectInitializer): UBlueprintFactory(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UAMS_DataCenter::StaticClass();

	ParentClass = SupportedClass;
}


UObject* UAMS_Factory_DataCenter::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
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
			UAMS_DataCenterBlueprint::StaticClass(),
			UAMS_DataCenterGeneratedBlueprint::StaticClass(),
			CallingContext);

		return NewAsset;
	}
}

bool UAMS_Factory_DataCenter::ConfigureProperties()
{
	ParentClass = SupportedClass;
    return true;
}
