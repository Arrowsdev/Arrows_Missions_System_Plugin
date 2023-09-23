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
