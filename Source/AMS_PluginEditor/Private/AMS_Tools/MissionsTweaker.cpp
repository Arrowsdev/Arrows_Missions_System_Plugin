//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_Tools/MissionsTweaker.h"
#include "AMS_PluginEditor/Public/AMS_Tools/MissionTweakerWindow.h"
//#include "AMS_PluginEditor.h"

void MissionsTweaker::Initialize()
{
	TabName = "Missions Tweaker";
	TabDisplayName = FText::FromString(TEXT("Missions Tweaker"));
	ToolTipText = FText::FromString("this is a central place to edit all missions default settings and variables\nso you dont need to open them class by class");
}

TSharedRef<SDockTab> MissionsTweaker::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
        .TabRole(ETabRole::DocumentTab)
        [
            SNew(MissionTweakerWindow)
            .TweakerTool(SharedThis(this))
        ];

    return SpawnedTab;
}
