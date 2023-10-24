//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAMS_PluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FAMS_PluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FAMS_PluginModule >("AMS_PluginModule");
	}

};
