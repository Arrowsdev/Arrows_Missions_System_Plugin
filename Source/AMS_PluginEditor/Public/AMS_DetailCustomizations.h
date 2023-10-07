//Advanced Mission System Developed by AIS Studios @2023

#pragma once
#include "IDetailCustomization.h"


class UMissionObject;
class UActionObject;

/**
 * customize mission object details to add hints and api docs 
 */
class AMS_PLUGINEDITOR_API Missions_DetailCustomizations : public IDetailCustomization
{
public:
   
    static TSharedRef<IDetailCustomization> MakeInstance();
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};

/**
 * customize Action object details to add hints and api docs
 */
class AMS_PLUGINEDITOR_API Action_DetailCustomizations : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance();
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};

