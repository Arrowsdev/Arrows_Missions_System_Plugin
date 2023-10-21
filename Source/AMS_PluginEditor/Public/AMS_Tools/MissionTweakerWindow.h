//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MissionsTweaker.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include "Widgets/SCompoundWidget.h"
#include "ClassIconFinder.h"
#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Input/Reply.h" 
#include "BlueprintEditorModule.h"
#include "UObject\SavePackage.h"
#include "AMS_PluginEditor.h"

struct FBrowserObject
{
public:
	TWeakObjectPtr<UObject> Object;

	//point to the bp asset that holds this object so we can open bp asset for this browserObject
	TWeakObjectPtr<UBlueprint> ObjectBP;
};

class IDetailsView;

/**
 * 
 */
class AMS_PLUGINEDITOR_API MissionTweakerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(MissionTweakerWindow) {}
	SLATE_ARGUMENT(TWeakPtr<class MissionsTweaker>, TweakerTool)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	TWeakPtr<MissionsTweaker> teakerTool;


private:
	
	//finds all missions assets 
	void RefreshList();

	//so we dont need to use the asset registry everytime we referesh we generate a ref list for the referesh to use 
	void GenerateList();

	void OnNewHostTextCommited(const FText& InText, ETextCommit::Type InCommitType);

	TSharedRef<ITableRow> HandleListGenerateRow(TSharedPtr<FBrowserObject> TransactionInfo, const TSharedRef<STableViewBase>& OwnerTable);

	void HandleListSelectionChanged(TSharedPtr<FBrowserObject> TransactionInfo, ESelectInfo::Type SelectInfo);

	FReply OnSavedClicked();
	FReply OnBuildClicked();
	


	void HandleIncludeAllCheckStateChange(ECheckBoxState newState);

	//used for searching for missions by name
	FText FilterText;
	FString FilterString;

	TArray< TSharedPtr<FBrowserObject> > LiveObjects;
	TArray< TSharedPtr<FBrowserObject> > GeneratedList;

	TSharedPtr< SListView< TSharedPtr<FBrowserObject> > > ObjectListView;

	TSharedPtr<IDetailsView> PropertyView;

public:
	TSharedPtr<STextBlock> TempPercentage;
	TSharedPtr<SCheckBox>  IncludeCB; //cb = check box
	
	//called when a row is checked so we see if all other rows are checked too
	void ValidateIncludeAll(ECheckBoxState newState);
};


//holds the selection, 
//@TODO : finish highlighting text when searching
class SObjectBrowserTableRow : public SMultiColumnTableRow<TSharedPtr<int32>>
{
public:

	SLATE_BEGIN_ARGS(SObjectBrowserTableRow) { }
	SLATE_ARGUMENT(TSharedPtr<FBrowserObject>, Object)
	SLATE_ARGUMENT(MissionTweakerWindow*, TweakerWindow)
	SLATE_ARGUMENT(FText, HighlightText)
	SLATE_END_ARGS()

public:

	SObjectBrowserTableRow()
		: ClassIcon(nullptr)
	{
	}

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		BrowserObject = InArgs._Object;
		
		if (UObject* Obj = InArgs._Object->Object.Get())
		{
			Name = FText::FromString(Obj->GetName());
			ClassName = Obj->GetClass()->GetDisplayNameText();
			Package = FText::FromString(Obj->GetFullName());
			HighlightText = InArgs._HighlightText;

			// Get selection icon from the selected object
			ClassIcon = FSlateIconFinder::FindIconForClass(Obj->GetClass()).GetIcon();

			TweakerWindow = InArgs._TweakerWindow;

			initialCheckState = Cast<UMissionObject>(Obj)->bIncludeInGame;
		}

		SMultiColumnTableRow<TSharedPtr<int32> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}

public:

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		if (ColumnName == NAME_Class)
		{
			return SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(ClassIcon)
				]

			+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(ClassName)
				];
		}
		else if (ColumnName == NAME_Package)
		{
			 return SNew(STextBlock)
			.Text(Package);
		}
		else if (ColumnName == FName("Include"))
		{
			return SAssignNew(IncludeState,SCheckBox)
				   .OnCheckStateChanged(this, &SObjectBrowserTableRow::OnCheckStateChanged)
				   .IsChecked(initialCheckState);
		}
		
		return SNullWidget::NullWidget;
	}

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override
	{
		if (UObject* LiveObject = BrowserObject->ObjectBP.Get())
		{
			if (GEditor)
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(LiveObject);
			}
		}

		return FReply::Handled();
	}

	inline void OnCheckStateChanged(ECheckBoxState newState)
	{
		Cast<UMissionObject>(BrowserObject->Object->GetClass()->GetDefaultObject())->bIncludeInGame = newState == ECheckBoxState::Checked;
		BrowserObject->ObjectBP->MarkPackageDirty();
		BrowserObject->Object->MarkPackageDirty();
		TweakerWindow->ValidateIncludeAll(newState);
	}

private:

	TSharedPtr<FBrowserObject> BrowserObject;

	const FSlateBrush* ClassIcon;

	MissionTweakerWindow* TweakerWindow;

	TSharedPtr<SCheckBox> IncludeState;

	bool initialCheckState;

	// Holds the transaction's title text.
	FText Name;
	FText ClassName;
	FText Package;

	FText HighlightText;
};

//class to do the building mission in a worker thread so if the game had many missions we dont want the building task to hang the editor
class FAMS_BackgroundTask : public FNonAbandonableTask
{
public:
	FAMS_BackgroundTask(TArray< TSharedPtr<FBrowserObject> > _LiveObjects, MissionTweakerWindow* _TweakerWindow):
		                LiveObjects(_LiveObjects), TweakerWindow(_TweakerWindow){}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FAMS_BackgroundTask, STATGROUP_ThreadPoolAsyncTasks); }

	void DoWork();

private:
	TArray< TSharedPtr<FBrowserObject> > LiveObjects;
	MissionTweakerWindow* TweakerWindow;
};