//Advanced Mission System Developed by AIS Studios @2023

#include "AssetTypes/AMS_AssetType_MissionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"

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

void UAMS_AssetType_MissionObject::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
   
    FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
   
}

uint32 UAMS_AssetType_MissionObject::GetCategories()
{
    return MyAssetCategory;
}

bool AMS_EditorUtilities::PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class)
{
   
    // TSharedPtr<FClassFilterObject> Filter = MakeShareable(new FClassFilterObject(UMissionObject::StaticClass));

    //Filter->AllowedChildrenOfClasses.Add(Class);

    //// Fill in options

    //FClassViewerInitializationOptions Options;

    //Options.Mode = EClassViewerMode::ClassPicker;

    //Options.ClassFilter = Filter;

    //Options.bShowUnloadedBlueprints = true;

    //Options.bExpandRootNodes = true;

    //Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::Dynamic;

    //return SClassPickerDialog::PickClass(TitleText, Options, OutChosenClass, Class);
    return false;
}
