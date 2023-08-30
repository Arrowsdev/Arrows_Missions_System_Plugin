//Advanced Mission System Developed by AIS Studios @2023

#include "AssetTypes/AMS_AssetType_MissionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include  "BlueprintEditorModule.h"
#include "Helpers/AMS_Utilites.h"
#include <KismetCompilerModule.h>


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAMS_AssetType_MissionObject::UAMS_AssetType_MissionObject(EAssetTypeCategories::Type InAssetCategory):
    MyAssetCategory(InAssetCategory)
{
}

FText UAMS_AssetType_MissionObject::GetName() const
{
    return FText::FromString("Mission Object");
}

FColor UAMS_AssetType_MissionObject::GetTypeColor() const
{
    return FColor::Cyan;
}

UClass* UAMS_AssetType_MissionObject::GetSupportedClass() const
{
    return UMissionObject::StaticClass();
}

FText UAMS_AssetType_MissionObject::GetAssetDescription(const FAssetData& AssetData) const
{
    return FText::FromString("Create New Or Drived Mission Object");
}

uint32 UAMS_AssetType_MissionObject::GetCategories()
{
    return MyAssetCategory;
}

void UAMS_AssetType_MissionObject::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{

	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>(Object))
		{
			bool bLetOpen = true;
			if (!Blueprint->SkeletonGeneratedClass || !Blueprint->GeneratedClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("FailedToLoadBlueprintWithContinue", "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?"));
			}
			if (bLetOpen)
			{
				FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
				TSharedRef< IBlueprintEditor > NewKismetEditor = BlueprintEditorModule.CreateBlueprintEditor(Mode, EditWithinLevelEditor, Blueprint, false);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadBlueprint",
		   "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}


#undef LOCTEXT_NAMESPACE 
