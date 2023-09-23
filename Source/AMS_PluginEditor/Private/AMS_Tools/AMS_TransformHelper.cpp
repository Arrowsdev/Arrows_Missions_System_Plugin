//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_Tools/AMS_TransformHelper.h"
#include "AMS_Tools/TransformHelperWindow.h"

void AMS_TransformHelper::Initialize()
{
    TabName = "Transform Helper";
    TabDisplayName = FText::FromString(TEXT("Transform Helper"));
    ToolTipText = FText::FromString("this is meant to replace the old transform helper pawn, for easer workflow");
}

TSharedRef<SDockTab> AMS_TransformHelper::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            SNew(STransformHelperWindow)
            .TransformHelper(SharedThis(this))
        ];

    return SpawnedTab;
}


//####### ABOUT SECTION #############
void AMS_About::Initialize()
{
    TabName = "About";
    TabDisplayName = FText::FromString(TEXT("About"));
    ToolTipText = FText::FromString("About");
}

TSharedRef<SDockTab> AMS_About::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
        .TabRole(ETabRole::DocumentTab)
        [
            SNew(SAboutWindow)
            .About(SharedThis(this))
        ];

    return SpawnedTab;
}
