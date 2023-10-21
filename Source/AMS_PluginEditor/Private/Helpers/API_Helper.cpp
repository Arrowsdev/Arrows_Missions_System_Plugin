// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/API_Helper.h"
#include "AMS_Plugin/Public/MissionObject.h"

using namespace AMS_Exposure;

void SAPI_Helper::Construct(const FArguments& InArgs)
{
	Src = InArgs._Src;
	Title = InArgs._Title;
	ExposeLevel = InArgs._ExposeLevel;

	if (ExposeLevel == EExposeLevel::simple)
	{

		ChildSlot
		[
			SNew(SBorder)
			[
			   SNew(SVerticalBox)
			   + SVerticalBox::Slot()
			   .AutoHeight()
			   [
					SNew(STextBlock)
					.Text(Title)
					.Justification(ETextJustify::Center)
			   ]
		       + SVerticalBox::Slot()
			   [
					SAssignNew(API_ListView, SListView<TSharedPtr<API_Expose>>)
					.ListItemsSource(&API_List)
					.SelectionMode(ESelectionMode::Single)
					.OnGenerateRow(this, &SAPI_Helper::OnGenerateRow)
					.HeaderRow
					(
						SNew(SHeaderRow)
						.Visibility(EVisibility::Visible)
						+ SHeaderRow::Column(FName("Number"))
						.DefaultLabel(FText::FromString("#"))
						.FixedWidth(20)
						+ SHeaderRow::Column(FName("Functions"))
						.DefaultLabel(FText::FromString("Functions"))
					)
				  ]
				]
			];

		GetAPI();
	}
	if (ExposeLevel == EExposeLevel::advanced)
	{
		ChildSlot
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
			    .AutoHeight()
			    [
					SNew(STextBlock)
					.Text(Title)
			        .Justification(ETextJustify::Center)
			    ]
			    + SVerticalBox::Slot()
			    [
					SNew(SSplitter)
					.Orientation(EOrientation::Orient_Horizontal)
					+ SSplitter::Slot()
					[
						SAssignNew(API_ListView, SListView<TSharedPtr<API_Expose>>)
						.ListItemsSource(&API_List)
					    .SelectionMode(ESelectionMode::Single)
					    .OnGenerateRow(this, &SAPI_Helper::OnGenerateRow)
					    .OnSelectionChanged(this, &SAPI_Helper::HandleListSelectionChanged)
						.HeaderRow
						(
							SNew(SHeaderRow)
							.Visibility(EVisibility::Visible)
							+ SHeaderRow::Column(FName("Number"))
							.DefaultLabel(FText::FromString("#"))
							.FixedWidth(20)
							+ SHeaderRow::Column(FName("Functions"))
							.DefaultLabel(FText::FromString("Functions"))
						)
					]
				    + SSplitter::Slot()
					[
						SAssignNew(API_ReporterView, SAPI_Reporter)
					]
				]
		       
			];

		GetAdvancedAPI();
	}
}

TSharedRef<ITableRow> SAPI_Helper::OnGenerateRow(TSharedPtr<API_Expose> api, const TSharedRef<STableViewBase>& ListView)
{
	return SNew(SAPI_Row, ListView)
		   .API(api);
}

void SAPI_Helper::HandleListSelectionChanged(TSharedPtr<API_Expose> api, ESelectInfo::Type SelectInfo)
{
	API_ReporterView->API = api;
	API_ReporterView->RequestRefresh();
}

void SAPI_Helper::GetAPI()
{
	API_List.Reset();

	int32 Num = 1;
	for (TFieldIterator<UFunction> FuncItr(Src); FuncItr; ++FuncItr)
	{
		UFunction* function = *FuncItr;

		if (!function) continue;

		FString FuncName = function->GetName();
		FString CalledWith = FuncName.Contains("ST_") ? "Called Without ( ST_ )" : "";
		FString Subfix = function->HasAnyFunctionFlags(FUNC_Static) ? FString::Printf(TEXT(" [ Static Function %s ]"), *CalledWith) : "";

		FuncName += Subfix;

		//get only normal callable functions or pure functions or Events
		if ((function->HasAnyFunctionFlags(FUNC_BlueprintCallable) || function->HasAnyFunctionFlags(FUNC_BlueprintPure)
			                  || function->HasAnyFunctionFlags(FUNC_BlueprintEvent)) && FuncName != "ExecuteUbergraph")
		{
			TSharedPtr<API_Expose> _Func = MakeShareable(new API_Expose(FText::FromString(FuncName), Num));
			API_List.Add(_Func);
			Num++;

		}
	}

	API_ListView->RequestListRefresh();
}

void SAPI_Helper::GetAdvancedAPI()
{
	API_List.Reset();

	int32 Num = 1;
	for (TFieldIterator<UFunction> FuncItr(Src); FuncItr; ++FuncItr)
	{
		UFunction* function = *FuncItr;

		if (!function) continue;

		FString FuncName = function->GetName();
		if ((function->HasAnyFunctionFlags(FUNC_BlueprintCallable) || function->HasAnyFunctionFlags(FUNC_BlueprintPure)
			|| function->HasAnyFunctionFlags(FUNC_BlueprintEvent)) && FuncName != "ExecuteUbergraph")
		{
			TSharedPtr<API_Expose> _Func = MakeShareable(new API_Expose(function, Num));
			API_List.Add(_Func);
			Num++;

		}
	}

	API_ListView->RequestListRefresh();
}



void SAPI_Reporter::Construct(const FArguments& InArgs)
{
	API = InArgs._API;

ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
	    .AutoHeight()
	    .Padding(5)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Comment and Tool Tip :"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.Padding(5)
				[
					SAssignNew(Comment, STextBlock)
					.AutoWrapText(true)
				    .Justification(ETextJustify::Left)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
		    .Padding(0.f, 5.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Parameters :"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.Padding(5)
				[
					SAssignNew(Parameters, STextBlock)
					.AutoWrapText(true)
				    .Justification(ETextJustify::Left)
				]
			]
		
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
		   SAssignNew(Extra, STextBlock)
		]
		
	];

   RequestRefresh();
}

void SAPI_Reporter::RequestRefresh()
{
	if (!API) return;
	Comment->SetText(API->Comment);
	Parameters->SetText(API->Parameters);
	Extra->SetText(API->Extra);
}
