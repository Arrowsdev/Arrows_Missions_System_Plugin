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
#include <BlueprintEditorModule.h>

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

	void OnNewHostTextCommited(const FText& InText, ETextCommit::Type InCommitType);

	TSharedRef<ITableRow> HandleListGenerateRow(TSharedPtr<FBrowserObject> TransactionInfo, const TSharedRef<STableViewBase>& OwnerTable);

	void HandleListSelectionChanged(TSharedPtr<FBrowserObject> TransactionInfo, ESelectInfo::Type SelectInfo);

	
	//used for searching for missions by name
	FText FilterText;
	FString FilterString;

	TArray< TSharedPtr<FBrowserObject> > LiveObjects;

	TSharedPtr< SListView< TSharedPtr<FBrowserObject> > > ObjectListView;

	TSharedPtr<IDetailsView> PropertyView;

};


//holds the selection, 
//@TODO : finish highlighting text when searching
class SObjectBrowserTableRow : public SMultiColumnTableRow<TSharedPtr<int32>>
{
public:

	SLATE_BEGIN_ARGS(SObjectBrowserTableRow) { }
	SLATE_ARGUMENT(TSharedPtr<FBrowserObject>, Object)
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

private:

	TSharedPtr<FBrowserObject> BrowserObject;

	const FSlateBrush* ClassIcon;

	// Holds the transaction's title text.
	FText Name;
	FText ClassName;
	FText Package;

	FText HighlightText;
};
