//Advanced Mission System Developed by AIS Studios @2023

#include "AMS_DetailCustomizations.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#include "AMS_Plugin/Public/ActionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"


#include "AMS_PluginEditor/Public/Helpers/API_Helper.h"

TSharedRef<IDetailCustomization> Missions_DetailCustomizations::MakeInstance()
{
	return MakeShareable(new Missions_DetailCustomizations);
}

void Missions_DetailCustomizations::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    DetailLayout.HideProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UMissionObject, APIHook)));

    IDetailCategoryBuilder& ArrowsGreeting = DetailLayout.EditCategory("Settings", FText::FromString(""), ECategoryPriority::Important);
    ArrowsGreeting.AddCustomRow(FText::FromString("MissionInfo"))
        .WholeRowContent()
        [
            SNew(SBorder)
            .Padding(5)
            [
               SNew(STextBlock)
              .AutoWrapText(true)
              .Text(FText::FromString("Thanks for using Arrows Tools.\nthis is the mission class and it has the main logics for mission progression and others , and the subsystem is responsible for managing and saving and loading logics"))
           ]

        ];

    IDetailCategoryBuilder& APIExposing = DetailLayout.EditCategory("API", FText::FromString(""), ECategoryPriority::Important);
    APIExposing.AddCustomRow(FText::FromString("Arrows Mission API"))
        .WholeRowContent()
        [
            SNew(SAPI_Helper)
            .Src(UMissionObject::StaticClass())
            .Title(FText::FromString("Mission API Calls"))
        ];

}


//####################### Actions ############################################
TSharedRef<IDetailCustomization> Action_DetailCustomizations::MakeInstance()
{
	return MakeShareable(new Action_DetailCustomizations);
}

void Action_DetailCustomizations::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
   
    DetailLayout.HideProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UActionObject, ActionInfo)));

    IDetailCategoryBuilder& ArrowsGreeting = DetailLayout.EditCategory("ActionInfo", FText::FromString(""), ECategoryPriority::Important);
    ArrowsGreeting.AddCustomRow(FText::FromString("ActionInfo"))
        .WholeRowContent()
        [
            SNew(SBorder)
            .Padding(5)
            [
                SNew(STextBlock)
                .AutoWrapText(true)
                .Text(FText::FromString("Thanks for using Arrows Tools.\nAction object is used to represent any game related action and by passing it in the preform function, it tells the system that this certain gameplay thing has happened and the mission can calcuate the progress according to the action count ,  you can use also tutorial actions for input monitering in control tutorial missions if you have one in your game!"))
            ]
           
        ];
}
