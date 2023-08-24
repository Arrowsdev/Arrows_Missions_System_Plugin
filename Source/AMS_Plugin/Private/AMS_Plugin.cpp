//Advanced Mission System Developed by AIS Studios @2023

#include "AMS_Plugin.h"

#define LOCTEXT_NAMESPACE "FAMS_PluginModule"

void FAMS_PluginModule::StartupModule()
{
	//should register the tick here
}

void FAMS_PluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAMS_PluginModule, AMS_Plugin)