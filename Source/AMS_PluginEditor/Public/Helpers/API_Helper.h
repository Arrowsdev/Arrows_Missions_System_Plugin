// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SCompoundWidget.h"

struct API_Expose
{
public:
	API_Expose(): Function(), Number() { };

	API_Expose(FText _function, int32 _number) : Function(_function)
	{
		Number = FString::FromInt(_number);
		Number += "-";
	}

	FText Function;
	FString Number;
};
/**
 * this class is meant to help exposing Source Class functions in a list form for beginners users to read instead of the docs
 */
class AMS_PLUGINEDITOR_API SAPI_Helper : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAPI_Helper) {}
	SLATE_ARGUMENT(UClass*, Src)
	SLATE_ARGUMENT(FText, Title)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TArray<TSharedPtr<API_Expose>> API_List;
	TSharedPtr< SListView<TSharedPtr<API_Expose>>> API_ListView;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<API_Expose> api, const TSharedRef<STableViewBase>& ListView);

	void GetAPI();
	UClass* Src;
	FText Title;
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