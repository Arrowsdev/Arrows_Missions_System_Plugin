// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/API_Helper.h"
#include "AMS_Plugin/Public/MissionObject.h"

void SAPI_Helper::Construct(const FArguments& InArgs)
{
	Src = InArgs._Src;
	Title = InArgs._Title;

	ChildSlot
		[
			SNew(SBorder)
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
					SAssignNew(API_ListView, SListView<TSharedPtr<API_Expose>>)
					.ListItemsSource(&API_List)
					.SelectionMode(ESelectionMode::None)
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

TSharedRef<ITableRow> SAPI_Helper::OnGenerateRow(TSharedPtr<API_Expose> api, const TSharedRef<STableViewBase>& ListView)
{
	return SNew(SAPI_Row, ListView)
		   .API(api);
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
