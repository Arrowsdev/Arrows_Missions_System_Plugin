//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "ModuleListenerInterface.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

/**
 *
 */
class AMS_PLUGINEDITOR_API MissionsTweaker : public IExampleModuleListenerInterface, public TSharedFromThis<MissionsTweaker>
{

public:

    virtual ~MissionsTweaker() {}

    //called from the editor module to set this tool information
    virtual void OnStartupModule() override
    {
        Initialize();
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &MissionsTweaker::SpawnTab))
            .SetGroup(FAMS_PluginEditor::Get().GetMenuRoot())
            .SetDisplayName(TabDisplayName)
            .SetTooltipText(ToolTipText);

        FAMS_PluginEditor::Get().AddMenuExtension(FMenuExtensionDelegate::CreateRaw(this, &MissionsTweaker::MakeMenuEntry), FName("Section_1"));
    };

    virtual void OnShutdownModule() override
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
    };


    virtual void Initialize();

    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs);

    virtual void MakeMenuEntry(FMenuBuilder& menuBuilder)
    {
        FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(menuBuilder, TabName);
    };

protected:
    FName TabName;
    FText TabDisplayName;
    FText ToolTipText;

};