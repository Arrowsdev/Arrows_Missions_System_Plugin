//Advanced Mission System Developed by AIS Studios @2023
#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"
#include "ToolWidgets/Public/SSearchableComboBox.h"
#include "AMS_TransformHelper.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include "AssetRegistry/AssetRegistryModule.h"


class UTypedElementSelectionSet;


///**
// * slate window for the transform helper tool 
// */
class AMS_PLUGINEDITOR_API STransformHelperWindow : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(STransformHelperWindow) {}
	SLATE_ARGUMENT(TWeakPtr<class AMS_TransformHelper>, TransformHelper)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	TWeakPtr<AMS_TransformHelper> transformHelper;

	FSlateBrush* Logo;

	TArray<TSharedPtr<FString>> ClassesOptions;
	TArray<TSharedPtr<FString>> ClassVariables;
	
	TSharedPtr<STextComboBox> VariableSelector;

	TSharedPtr<SBorder> ResultsBorder;
	TSharedPtr<STextBlock> ResultsText;

	TSharedPtr<STextBlock> WarningText;


	//list of names and cdo's , changed to save the classes instead of the cdo cuz that was causing crash if the user opened any class editor 
	//while the transform helper is opened, my guess that when that class compiles the generated class will have new address so the stored cdo will be not valid
	TMap<FString, TSubclassOf<UMissionObject>> MissionsList;
	
	//selected names used to pull the correct cdo from the map
	FString SelectedVariableName;
	FString SelectedMission;

	//used to hold the correct type of the property so we dont need to check again when we want to edit it
	struct PropertyTypePair
	{
		PropertyTypePair(FProperty* prop, bool isSingle):_Property(prop), IsSingle(isSingle) {};
		
		FProperty* _Property;
		bool IsSingle;
	};

	enum HelperState
	{
		AllGood,
		AllBad
	};

	TMap<FString, PropertyTypePair> Typed_VariablesList;

	void GenerateClassesOptions();
	
	void OnMissionSelectionChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo);
	void OnVariableSelectionChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo);

	void GenerateVariableListForSelection(TSharedPtr<FString> Selection);

	bool CheckArrayPropertyType(FArrayProperty* property);

	FReply OnApplyButtonClicked();

	//returns the world transform for each one of the selected actors in the world
	TArray<FTransform> GetSelectionActorsTransforms();

	//used to remove the unwanted prefixes in classes names : Default__ClassName_C ==> ClassName
	FString CleanString(FString string);


	UTypedElementSelectionSet* GetWorldSelectionSet();

	void ShowMessage(FString message, HelperState state);

	FString ParseTransformString(FTransform transform);
};

class AMS_PLUGINEDITOR_API SAboutWindow : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAboutWindow) {}
	SLATE_ARGUMENT(TWeakPtr<class AMS_About>, About)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	TWeakPtr<AMS_About> about;

};