//Advanced Mission System Developed by AIS Studios @2023


#include "AssetTypes/AMS_AssetType_SaveGame.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"

UAMS_AssetType_SaveGame::UAMS_AssetType_SaveGame(EAssetTypeCategories::Type InAssetCategory):
     MyAssetCategory(InAssetCategory)
{
}

FText UAMS_AssetType_SaveGame::GetName() const
{
    return FText::FromString("AMS Save Game");
}

FColor UAMS_AssetType_SaveGame::GetTypeColor() const
{
    return FColor::Blue;
}

UClass* UAMS_AssetType_SaveGame::GetSupportedClass() const
{
    return UAMS_SaveGame::StaticClass();
}

void UAMS_AssetType_SaveGame::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

uint32 UAMS_AssetType_SaveGame::GetCategories()
{
    return MyAssetCategory;
}
