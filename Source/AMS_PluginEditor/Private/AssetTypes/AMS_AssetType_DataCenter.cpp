// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypes/AMS_AssetType_DataCenter.h"
#include "AMS_Plugin/System/AMS_DataCenter.h"

UAMS_AssetType_DataCenter::UAMS_AssetType_DataCenter(EAssetTypeCategories::Type InAssetCategory):
MyAssetCategory(InAssetCategory)
{
}

UClass* UAMS_AssetType_DataCenter::GetSupportedClass() const
{
	return UAMS_DataCenter::StaticClass();
}
