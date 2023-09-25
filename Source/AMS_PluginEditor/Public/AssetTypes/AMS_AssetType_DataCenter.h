// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
/**
 * 
 */
class AMS_PLUGINEDITOR_API UAMS_AssetType_DataCenter : public FAssetTypeActions_Base
{
public:
	UAMS_AssetType_DataCenter(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_DataCenter() {};

	virtual FText GetName() const override { return FText::FromString("Data Center"); };
	virtual FColor GetTypeColor() const override { return FColor::Magenta; };
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override { return MyAssetCategory; };


private:

	EAssetTypeCategories::Type MyAssetCategory;

};
