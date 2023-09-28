//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_Tools/MissionTweakerWindow.h"
#include "ClassViewerModule.h"
#include "Kismet2/SClassPickerDialog.h"
#include "EditorFontGlyphs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SourceControlHelpers.h"

void MissionTweakerWindow::Construct(const FArguments& InArgs)
{
	teakerTool = InArgs._TweakerTool;

	//Get Property Module reference
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//detail pannel view setup
	FDetailsViewArgs DetailsArgs = FDetailsViewArgs();

	DetailsArgs.bUpdatesFromSelection = false;
	DetailsArgs.bLockable = false;
	DetailsArgs.bAllowSearch =  true;
	DetailsArgs.FDetailsViewArgs::ObjectsUseNameArea;
	DetailsArgs.bHideSelectionTip = true;
	DetailsArgs.NotifyHook = nullptr;
	DetailsArgs.bSearchInitialKeyFocus = false;
	DetailsArgs.ViewIdentifier = NAME_None;
	DetailsArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;

	PropertyView = EditModule.CreateDetailView(DetailsArgs);

	
ChildSlot
[
	SNew(SVerticalBox)
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.AutoHeight()
	[
		SNew(SBox)
		.WidthOverride(100)
	    .HAlign(HAlign_Left)
		[
			SNew(SButton)
			.Text(FText::FromString("Save All"))
	        .OnClicked(this, &MissionTweakerWindow::OnSavedClicked)
		]
	
	]
   + SVerticalBox::Slot()
   .VAlign(VAlign_Fill)
   [
		   SNew(SSplitter)
		   .Orientation(EOrientation::Orient_Horizontal)

		   + SSplitter::Slot()
		   .Value(3)
		   [
			   SNew(SBorder)
			   .Padding(FMargin(3))
		   .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		   [
			   SNew(SVerticalBox)
			   + SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
		   .FillWidth(1.0f)
		   [
			   SNew(SEditableTextBox)
			   .HintText(FText::FromString("Mission Name"))
		   .OnTextCommitted(this, &MissionTweakerWindow::OnNewHostTextCommited)
		   .OnTextChanged(this, &MissionTweakerWindow::OnNewHostTextCommited, ETextCommit::Default)
		   ]
	   ]
       + SVerticalBox::Slot()
	   .FillHeight(1.0f)
	   [
		   SNew(SBorder)
		   .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
	       .Padding(FMargin(0.0f, 4.0f))
		   [
			   SAssignNew(ObjectListView, SListView< TSharedPtr<FBrowserObject> >)
			   .ItemHeight(24.0f)
			   .ListItemsSource(&LiveObjects)
			   .SelectionMode(ESelectionMode::Single)
			   .OnGenerateRow(this, &MissionTweakerWindow::HandleListGenerateRow)
			   .OnSelectionChanged(this, &MissionTweakerWindow::HandleListSelectionChanged)
			   .HeaderRow
			   (
				   SNew(SHeaderRow)
				   .Visibility(EVisibility::Visible)
				   + SHeaderRow::Column(NAME_Class)
				   .DefaultLabel(FText::FromString("Mission"))
				   + SHeaderRow::Column(NAME_Package)
				   .DefaultLabel(FText::FromString("Path"))
			   )
	       ]
	   ]
	   ]
	   ]

       + SSplitter::Slot()
	   .Value(2)
	   [
		   SNew(SBorder)
		   .Padding(FMargin(3))
	   .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
	   [
		   PropertyView.ToSharedRef()
	   ]
	   ]
   ]
		
	];

	RefreshList();
}


void MissionTweakerWindow::RefreshList()
{
	
	LiveObjects.Reset();

	TArray<FAssetData> FoundMissions;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FTopLevelAssetPath BaseClassName = UMissionObject::StaticClass()->GetDefaultObject()->GetClass()->GetClassPathName();

	TArray<FName>Tags;
	Tags.Add("MissionObject");
	AssetRegistryModule.Get().GetAssetsByTags(Tags, FoundMissions);


	if (FoundMissions.Num() > 0)
	{
		for (FAssetData& mission : FoundMissions)
		{
			UBlueprint* MissionBlueprint = Cast<UBlueprint>(mission.GetAsset());
			if (MissionBlueprint)
			{
				
				UMissionObject* missionObject = Cast<UMissionObject>(MissionBlueprint->GeneratedClass.GetDefaultObject());
				if (missionObject)
				{
					FString ObjectName = missionObject->GetClass()->GetDisplayNameText().ToString();
					if (ObjectName.Contains(FilterString) || FilterString.IsEmpty())
					{
						TSharedPtr<FBrowserObject> NewObject = MakeShareable(new FBrowserObject());
						NewObject->Object = missionObject;
						NewObject->ObjectBP = MissionBlueprint;
						LiveObjects.Add(NewObject);
					}
					
				}
			}
		}
	}

	ObjectListView->RequestListRefresh();
}


void MissionTweakerWindow::OnNewHostTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	FilterText = InText;
	FilterString = FilterText.ToString();

	RefreshList();
}

TSharedRef<ITableRow> MissionTweakerWindow::HandleListGenerateRow(TSharedPtr<FBrowserObject> TransactionInfo, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SObjectBrowserTableRow, OwnerTable)
		.Object(TransactionInfo)
		.HighlightText(FilterText);
}

void MissionTweakerWindow::HandleListSelectionChanged(TSharedPtr<FBrowserObject> TransactionInfo, ESelectInfo::Type SelectInfo)
{
	if (!TransactionInfo.IsValid())
	{
		return;
	}

	TArray< TWeakObjectPtr<UObject> > Selection;
	Selection.Add(TransactionInfo->Object);

	if (PropertyView)
	{
		PropertyView->SetObjects(Selection);
		UE_LOG(LogTemp, Warning, TEXT("selected property was set to %s"), *TransactionInfo->Object->GetName());
	}
		
	else
		UE_LOG(LogTemp, Warning, TEXT("no property view exsist"));
}

FReply MissionTweakerWindow::OnSavedClicked()
{
	TArray<FAssetData> FoundMissions;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FTopLevelAssetPath BaseClassName = UMissionObject::StaticClass()->GetDefaultObject()->GetClass()->GetClassPathName();

	TArray<FName>Tags;
	Tags.Add("MissionObject");
	AssetRegistryModule.Get().GetAssetsByTags(Tags, FoundMissions);


	if (FoundMissions.Num() > 0)
	{
		for (FAssetData& mission : FoundMissions)
		{
			UBlueprint* MissionBlueprint = Cast<UBlueprint>(mission.GetAsset());
			if (MissionBlueprint)
			{		
				FSavePackageArgs saveArgs;
				saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
				saveArgs.SaveFlags = SAVE_NoError;

				UPackage* Package = MissionBlueprint->GetPackage();

				FString NameStr = USourceControlHelpers::PackageFilename(Package);
				const TCHAR* FileName = *NameStr;

				if (Package->IsDirty())
				{
					if(UPackage::SavePackage(Package, MissionBlueprint, FileName, saveArgs))
					UE_LOG(LogTemp, Warning, TEXT("Mission Saved : %s"), *MissionBlueprint->GetName())

					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Faild To Save Mission %s"), *MissionBlueprint->GetName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%s Mission Has No Changes"), *MissionBlueprint->GetName());
				}
			}
		}
	}

	return FReply::Handled();
}
