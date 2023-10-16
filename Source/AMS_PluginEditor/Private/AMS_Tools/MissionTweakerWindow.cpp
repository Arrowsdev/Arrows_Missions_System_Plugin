//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_Tools/MissionTweakerWindow.h"
#include "ClassViewerModule.h"
#include "Kismet2/SClassPickerDialog.h"
#include "EditorFontGlyphs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SourceControlHelpers.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

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

	FButtonStyle* SaveButtonStyle = new FButtonStyle();
	SaveButtonStyle->SetNormal(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("SaveButton"));
	SaveButtonStyle->SetHovered(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("SaveButton"));
	SaveButtonStyle->SetPressed(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("SaveButton"));
	SaveButtonStyle->HoveredForeground = FColor::Cyan;

	FButtonStyle* BuildMissionsListStyle = new FButtonStyle();
	BuildMissionsListStyle->SetNormal(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("BuildList"));
	BuildMissionsListStyle->SetHovered(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("BuildList"));
	BuildMissionsListStyle->SetPressed(*FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("BuildList"));
	BuildMissionsListStyle->HoveredForeground = FColor::Cyan;

	const FSlateBrush* SearchIco = FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush("SearchIcon");

ChildSlot
[
	SNew(SVerticalBox)
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.AutoHeight()
	[
		SNew(SBorder)
		[
          SNew(SHorizontalBox)
		  +SHorizontalBox::Slot()
	      .HAlign(HAlign_Left)
	      .AutoWidth()
		  [
			  SNew(SButton)
			  .OnClicked(this, &MissionTweakerWindow::OnSavedClicked)
	          .ButtonStyle(SaveButtonStyle)
	          .ToolTipText(FText::FromString("Save All Changes"))
		  ]
		    + SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SButton)
				.OnClicked(this, &MissionTweakerWindow::OnBuildClicked)
				.ButtonStyle(BuildMissionsListStyle)
				.ToolTipText(FText::FromString("Build Full Missions List, those are the missions that will be included in the playable missions\n not meaning they wont be cooked, they just wont have a record, delete them if you dont want them cooked!\nDO NOT CLOSE THE TWEAKER UNTILL THE BUILD IS FINISHED!"))
			]
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
	           .AutoWidth()
			   [
			      SNew(SHorizontalBox)
				  + SHorizontalBox::Slot()
	               .AutoWidth()
	               .HAlign(HAlign_Left)
	               [
					   SAssignNew(IncludeCB,SCheckBox)
					   .OnCheckStateChanged(this, &MissionTweakerWindow::HandleIncludeAllCheckStateChange)
				   ]
			       + SHorizontalBox::Slot()
				   .AutoWidth()
				   .HAlign(HAlign_Left)
				   .VAlign(VAlign_Center)
				   [
					   SNew(STextBlock)
					   .Text(FText::FromString("Include All"))
					   .Justification(ETextJustify::Left)
				   ]

			   ]
			   + SHorizontalBox::Slot()
			   .FillWidth(1.0f)
			   .Padding(5.f, 0.f, 0.f, 0.f)
			   [
				   SNew(SHorizontalBox)
				   +SHorizontalBox::Slot()
				   .AutoWidth()
				   [
					   SNew(SImage)
					   .Image(SearchIco)
				   ]
			        + SHorizontalBox::Slot()
					.FillWidth(1.f)
				   [
					   SNew(SEditableTextBox)
					   .HintText(FText::FromString("Mission Name"))
					   .OnTextCommitted(this, &MissionTweakerWindow::OnNewHostTextCommited)
					   .OnTextChanged(this, &MissionTweakerWindow::OnNewHostTextCommited, ETextCommit::Default)
				   ]
				   
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
				   + SHeaderRow::Column(FName("Include"))
				   .DefaultLabel(FText::FromString("In"))
				   .FixedWidth(20)
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
   + SVerticalBox::Slot()
   .AutoHeight()
   [
	   SAssignNew(TempPercentage, STextBlock)
	   .Text(FText::FromString("0%"))
   ]
		
];

    GenerateList();
	RefreshList();
}


void MissionTweakerWindow::RefreshList()
{
	
	LiveObjects.Reset();

	for(auto & itr : GeneratedList)
	{
		FString ObjectName = itr.Get()->Object->GetClass()->GetDisplayNameText().ToString();
		if (ObjectName.Contains(FilterString) || FilterString.IsEmpty())
		{
			LiveObjects.Add(itr);
		}
	}

	ObjectListView->RequestListRefresh();
	UE_LOG(LogTemp, Warning, TEXT("List Was Refreshed"));
}


void MissionTweakerWindow::GenerateList()
{
	GeneratedList.Reset();
	TArray<FAssetData> FoundMissions;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FTopLevelAssetPath BaseClassName = UMissionObject::StaticClass()->GetDefaultObject()->GetClass()->GetClassPathName();

	TArray<FName>Tags;
	Tags.Add("MissionObject");
	AssetRegistryModule.Get().GetAssetsByTags(Tags, FoundMissions);

	bool bAllIncluded = false;
	int32 includeCount = 0;

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
					TSharedPtr<FBrowserObject> NewObject = MakeShareable(new FBrowserObject());
					NewObject->Object = missionObject;
					NewObject->ObjectBP = MissionBlueprint;
					GeneratedList.Add(NewObject);
					if (missionObject->bIncludeInGame)
					{
						includeCount++;
					}
				}
			}
		}

		bAllIncluded = includeCount == FoundMissions.Num();
		IncludeCB.Get()->SetIsChecked(bAllIncluded);
	}
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
		.TweakerWindow(this)
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

FReply MissionTweakerWindow::OnBuildClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Build Started.."));
	(new FAutoDeleteAsyncTask<FAMS_BackgroundTask>(LiveObjects))->StartBackgroundTask();
	return FReply::Handled();
}

void MissionTweakerWindow::HandleIncludeAllCheckStateChange(ECheckBoxState newState)
{
	if (newState == ECheckBoxState::Checked)
	{
		for (auto& itr : GeneratedList)
		{
			Cast<UMissionObject>(itr->Object->GetClass()->GetDefaultObject())->bIncludeInGame = true;
			itr->Object->MarkPackageDirty();
			itr->ObjectBP->MarkPackageDirty();
		}

		GenerateList();//so the instances in the memory reflect the edited CDO
		RefreshList();
	}
}

void MissionTweakerWindow::ValidateIncludeAll(ECheckBoxState newState)
{
	bool AssumeAllChecked = true;

	for (auto& itr : GeneratedList)
	{
		if (Cast<UMissionObject>(itr->Object)->bIncludeInGame == false)
		{
			AssumeAllChecked = false;
			break;
		}
	}

	IncludeCB.Get()->SetIsChecked(AssumeAllChecked);
	
}


void FAMS_BackgroundTask::DoWork()
{
	UAMS_SubSystem* SubSystemDefaults = Cast<UAMS_SubSystem>(UAMS_SubSystem::StaticClass()->GetDefaultObject());
	SubSystemDefaults->ResetMissionsList();

	float Total = LiveObjects.Num();
	float At = 0.f;

	for (auto& obj : LiveObjects)
	{
		UMissionObject* ObjCDO = Cast<UMissionObject>(obj->Object.Get()->GetClass()->GetDefaultObject());
		if (ObjCDO->bIncludeInGame)
		{
			TSoftClassPtr<UMissionObject> NewEntry = obj->Object.Get()->GetClass();
			SubSystemDefaults->AddMissionToList(NewEntry);
		}
		At += 1.0f;
		UE_LOG(LogTemp, Warning, TEXT("[Building Missions List ] %f %"), (At / Total));
	}

	UE_LOG(LogTemp, Warning, TEXT("Build Finished"));
	SubSystemDefaults->SaveConfig();
}
