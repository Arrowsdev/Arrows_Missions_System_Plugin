//Advanced Mission System Developed by AIS Studios @2023


#include "AssetTypes/AMS_AssetType_SaveGame.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"

UAMS_AssetType_SaveGame::UAMS_AssetType_SaveGame(EAssetTypeCategories::Type InAssetCategory):
MyAssetCategory(InAssetCategory)
{
}

UClass* UAMS_AssetType_SaveGame::GetSupportedClass() const
{
    return UAMS_SaveGame::StaticClass();
}
