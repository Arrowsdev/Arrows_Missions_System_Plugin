// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

namespace AMS_Exposure
{
	enum EExposeLevel
	{
		simple,
		advanced
	};
}


struct API_Expose
{
public:
	API_Expose(): Function(), Number() { };

	//simple expose constructor
	API_Expose(FText _function, int32 _number) : Function(_function)
	{
		Number = FString::FromInt(_number);
		Number += "-";
	}

	//advanced expose constructor
	API_Expose(UFunction* _function, int32 _number)
	{
		Number = FString::FromInt(_number);
		Number += "-";

		Function = FText::FromString(_function->GetName());
		Comment = _function->GetToolTipText(false);

	    CalledWith = _function->GetName().Contains("ST_") ? "Called Without ( ST_ )" : "";
		Extra = _function->HasAnyFunctionFlags(FUNC_Static) ? FText::FromString(FString::Printf(TEXT("\n[ Static Function %s ]"), *CalledWith)) : FText();

	}

	FText Function;
	FText Comment;
	FText Extra;

	FString Number;
	FString CalledWith;
	
};

class SAPI_Reporter : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SAPI_Reporter) { }
	SLATE_ARGUMENT(TSharedPtr<API_Expose>, API)
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

	void RequestRefresh();

	TSharedPtr<API_Expose> API;

	TSharedPtr<STextBlock> Comment;
	TSharedPtr<STextBlock> Extra;

};

using namespace AMS_Exposure;
/**
 * this class is meant to help exposing Source Class functions in a list form for beginners users to read instead of the docs
 */
class AMS_PLUGINEDITOR_API SAPI_Helper : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAPI_Helper) {}
	SLATE_ARGUMENT(UClass*, Src)
	SLATE_ARGUMENT(FText, Title)
	SLATE_ARGUMENT(EExposeLevel, ExposeLevel)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TArray<TSharedPtr<API_Expose>> API_List;
	TSharedPtr< SListView<TSharedPtr<API_Expose>>> API_ListView;

	TSharedPtr <SAPI_Reporter> API_ReporterView;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<API_Expose> api, const TSharedRef<STableViewBase>& ListView);
	void HandleListSelectionChanged(TSharedPtr<API_Expose> api , ESelectInfo::Type SelectInfo);

	void GetAPI();
	void GetAdvancedAPI();

	UClass* Src;
	FText Title;
	EExposeLevel ExposeLevel;
};


class SAPI_Row : public SMultiColumnTableRow<TSharedPtr<int32>>
{
public:

	SLATE_BEGIN_ARGS(SAPI_Row) { }
	SLATE_ARGUMENT(TSharedPtr<API_Expose>, API)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		API = InArgs._API;
		SMultiColumnTableRow<TSharedPtr<int32> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}


	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		if (ColumnName == FName("Number"))
		{
			return  SNew(STextBlock)
				    .Text(FText::FromString(API.Get()->Number));
		}
		else
		{
			return SNew(STextBlock)
				  .Text(API.Get()->Function);
		}
		
	}

	TSharedPtr<API_Expose> API;
};

