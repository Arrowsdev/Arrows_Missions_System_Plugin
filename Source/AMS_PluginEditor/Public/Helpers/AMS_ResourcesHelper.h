//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"

/**
 *  class to hold icons for the action to be able to switch dynamically 
 */
class AMS_PLUGINEDITOR_API AMS_ResourcesHelper
{
public:
	AMS_ResourcesHelper();
	~AMS_ResourcesHelper();

	AMS_ResourcesHelper(FString resPath);

	void RegisterTexture(FString& resPath, TObjectPtr<UTexture2D>& outResults);

	TObjectPtr<UTexture2D> Do_Texture;
	TObjectPtr<UTexture2D> Dont_Texture;

};
