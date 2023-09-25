//Advanced Mission System Developed by AIS Studios @2023


#include "AMS_Tools/TransformHelperWindow.h"
#include "LevelEditor.h"
#include "Elements/Framework/TypedElementSelectionSet.h"


/*
* @TODO : add search logics so user can filter classes list to find desired class easieir
* @TODO : generailze the logic so it also can set variables in actions if the user wanted to set variables there
* @TODO : add the ability to tag selected actors with the selected class tag if the class was mission ofc, so it become easy to associate actors to mission
*/

//cuz slate confuses me i will create each element as a macro this will make it easy to pack them together

//this creates the tool info box
#define TOOL_INFO()\
    SNew(SBorder)\
    .BorderBackgroundColor(FColor(192, 192, 192, 255))\
    .Padding(15.0f)\
    [\
        SNew(STextBlock)\
        .Text(FText::FromString(TEXT(" select actors in the world so they can represent the transforms list to grab.\n then select a mission class and select the transform variable to set")))\
    ]\



//this is the class selector section of the window , it chooses a class so we can set it's variables 
#define CLASS_SELECTOR()\
 SNew(STextComboBox)\
.OptionsSource(&ClassesOptions)\
.OnSelectionChanged(this, &STransformHelperWindow::OnMissionSelectionChanged)\

#define VARIABLE_SELECTOR()\
SAssignNew(VariableSelector,STextComboBox)\
.OptionsSource(&ClassVariables)\
.OnSelectionChanged(this, &STransformHelperWindow::OnVariableSelectionChanged)\

#define APPLY_TRANSFORM()\
    SNew(SBox)\
    .MinDesiredWidth(200)\
    .HAlign(HAlign_Right)\
    [\
     SNew(SButton)\
    .Text(FText::FromString(TEXT("Send Transforms to variables")))\
    .OnClicked(this, &STransformHelperWindow::OnApplyButtonClicked)\
    ]\

#define SECTION(SectionName, SectionMacro)\
 SNew(SVerticalBox)\
+ SVerticalBox::Slot()\
[\
    SNew(SBorder)\
    [\
        SNew(STextBlock)\
        .Text(FText::FromString(TEXT(SectionName)))\
        .Justification(ETextJustify::Center)\
    ]\
]\
+ SVerticalBox::Slot()\
[\
    SectionMacro\
]\

void STransformHelperWindow::Construct(const FArguments& InArgs)
{
   
    FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
    FontInfo.Size = 24.f;
   
    FTextBlockStyle AIS_Style = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("BoldText");

    FSlateColor AIS_Color = FSlateColor(FColor(255, 255, 255, 100));

    transformHelper = InArgs._TransformHelper;
    if (transformHelper.IsValid())
    {
        
    }
   
    GenerateClassesOptions();

 ChildSlot
  [//CHILD SLOT START
       //main frame
       SNew(SBorder)
       .BorderBackgroundColor(FColor(192, 192, 192, 255))
       .Padding(15.0f)
       .VAlign(VAlign_Fill)
      [//FRAME START
       SNew(SVerticalBox)
       + SVerticalBox::Slot()
        .VAlign(VAlign_Fill)
        [//#1
            SNew(SScrollBox)
            + SScrollBox::Slot()
            [
                TOOL_INFO()
            ]
            + SScrollBox::Slot()
            .Padding(15.0f)
            [
               SNew(SHorizontalBox)
               +SHorizontalBox::Slot()
                .Padding(0.0f, 15.0f, 5.0f, 0.0f)
                [
                    SECTION("Select Class", CLASS_SELECTOR())  
                ]
                + SHorizontalBox::Slot()
                .Padding(0.0f, 15.0f, 5.0f, 0.0f)
                [
                   SECTION("Select Variable", VARIABLE_SELECTOR()) 
                ]
                + SHorizontalBox::Slot()
                .Padding(5.0f, 15.0f, 0.0f, 0.0f)
                [
                   SECTION("Apply", APPLY_TRANSFORM())      
                ]
              
            ]
        ]//#1

       + SVerticalBox::Slot()
        [//#2

           SAssignNew(ResultsBorder,SBorder)
           .BorderBackgroundColor(FColor(192, 192, 192, 0))
           .Padding(15.0f)
           [
               SAssignNew(ResultsText,STextBlock)
               .Text(FText::FromString(TEXT(" ")))
           ]

        ]//#2

       + SVerticalBox::Slot()
       .VAlign(VAlign_Bottom)
       .HAlign(HAlign_Right)
        [//#3
           SNew(SVerticalBox)
           + SVerticalBox::Slot()
           .HAlign(HAlign_Center)
           .VAlign(VAlign_Bottom)
           [
           SNew(STextBlock)
           .Text(FText::FromString(TEXT("ARROWS TOOLS")))
           .Justification(ETextJustify::Center)
           .Font(FontInfo)
           .TextStyle(&AIS_Style)
           .ColorAndOpacity(AIS_Color)
           ]
        ]//#3
      ]//FRAME END
     
   ];//CHILD SLOT END
}


void STransformHelperWindow::GenerateClassesOptions()
{
    MissionsList.Empty();

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
                    FString newSelection = CleanString(missionObject->GetName());
                    ClassesOptions.Add(MakeShareable(new FString(newSelection)));

                    MissionsList.Add(newSelection, missionObject->GetClass());
                }
            }
        }
    }
}

FString STransformHelperWindow::CleanString(FString string)
{
    FString src = string;
    FString Prefx = TEXT("Default__");
    FString Subfix = TEXT("_C");
    FString STo = TEXT("");
    const TCHAR* CPrefx = *Prefx;
    const TCHAR* CSubfix = *Subfix;
    const TCHAR* To = *STo;
    src = src.Replace(CPrefx, To);
    src = src.Replace(CSubfix, To);
    return src;
}


void STransformHelperWindow::OnMissionSelectionChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
    VariableSelector->ClearSelection();
    ClassVariables.Empty();
    SelectedVariableName = FString();

    if (Selection)
    {
        SelectedMission = *Selection;
        GenerateVariableListForSelection(Selection);
        UE_LOG(LogTemp, Warning, TEXT("Mission Selection has Changed to %s"), *(*Selection))
    }
}

void STransformHelperWindow::OnVariableSelectionChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
    if (Selection)
    {
        SelectedVariableName = *Selection;
        UE_LOG(LogTemp, Warning, TEXT("Variable Selection has Changed to %s"), *(*Selection))
    }
   
}


void STransformHelperWindow::GenerateVariableListForSelection(TSharedPtr<FString> Selection)
{

    UMissionObject* SelectedMissionCDO = MissionsList[*Selection].GetDefaultObject();

    TMap<FProperty*, bool> GenericTransformType;
    FProperty* prop = SelectedMissionCDO->GetClass()->PropertyLink;

    //get all transform variables, (Arrays and single transforms)
    while (prop)
    {
        if ((prop->GetClass() == FStructProperty::StaticClass() || prop->GetClass() == FArrayProperty::StaticClass()) && !prop->IsNative())
        {
            FStructProperty* AsStruct = CastField<FStructProperty>(prop);
            FArrayProperty* AsArrayStruct = CastField<FArrayProperty>(prop);

            //making sure the single struct we found is of type transform
            bool bwasSingleStruct = AsStruct ? AsStruct->Struct->GetFName() == NAME_Transform : false;
            bool bIsUberGraph = prop->GetFName() == FName("UberGraphFrame") ? true : false;
            bool bIsTransformArray = CheckArrayPropertyType(AsArrayStruct);

            //taking only "single transfoms" OR "array of transforms" , and ignoring the uber graph
            if ((bwasSingleStruct || bIsTransformArray) && !bIsUberGraph)
            GenericTransformType.Add(prop, bwasSingleStruct);
        }
        prop = prop->PropertyLinkNext;
    }

    //make names list
    for (auto& itr : GenericTransformType)
     {
        FString propertyName = itr.Key->GetName();
        ClassVariables.Add((MakeShareable(new FString(propertyName))));
        Typed_VariablesList.Add(propertyName, PropertyTypePair(itr.Key, itr.Value));//we dont need the property here anymore
     }
  
    if(GenericTransformType.Num() == 0)
      ClassVariables.Add((MakeShareable(new FString("No Transforms Found"))));

    //populate results to ui
    VariableSelector->RefreshOptions();
}

//making sure the type of array element is a transform struct so we dont get all arrays when we generate vars list
bool STransformHelperWindow::CheckArrayPropertyType(FArrayProperty* property)
{
    if (!property) return false;

    FStructProperty* InnerType = CastField<FStructProperty>(property->Inner);
    if (InnerType)
    {
        return InnerType->Struct->GetFName() == NAME_Transform;
    }

    return false;
}

FReply STransformHelperWindow::OnApplyButtonClicked()
{
    if (SelectedVariableName == FString() || SelectedMission == FString())
    {
        ShowMessage("Make Sure You Have Selected A Mission And A Property", AllBad);
        return FReply::Handled();
    }

    TArray<FTransform> list = GetSelectionActorsTransforms();

    if (list.Num() == 0)
    {
        ShowMessage(FString::Printf(TEXT("No Actors Selected in the levele! .\nPlease Select actor or set of actors and try again.")), AllBad);
        return FReply::Handled();
    }

    bool IsSingle = Typed_VariablesList[SelectedVariableName].IsSingle;
 
    UMissionObject* MissionCDO = MissionsList[SelectedMission].GetDefaultObject();

    if (IsSingle)
    {
        FStructProperty* StructProperty = FindFProperty<FStructProperty>(MissionCDO->GetClass(), FName(SelectedVariableName));
        FTransform* ValuePtr = StructProperty->ContainerPtrToValuePtr<FTransform>(MissionCDO);
        if (ValuePtr)
        {
        
            *ValuePtr = list[0];
        }

        FString ValueString = ParseTransformString(*ValuePtr);
        ShowMessage(FString::Printf(TEXT("Single transform varaible was set. \nVariable Name : %s \nvalue :\n %s"), *SelectedVariableName, *ValueString), AllGood);
    }

    else
    {
        FArrayProperty* StructProperty = FindFProperty<FArrayProperty>(MissionCDO->GetClass(), FName(SelectedVariableName));
        TArray<FTransform>* ArrValuePtr = StructProperty->ContainerPtrToValuePtr<TArray<FTransform>>(MissionCDO);
      
        ArrValuePtr->Empty();//remove old data in that array

        for (auto& transform : list)
        {
            ArrValuePtr->Add(transform);
        }

        ShowMessage(FString::Printf(TEXT("Array transform varaible was set. \nVariable Name : %s \nNumber of Entries : %d"), *SelectedVariableName, ArrValuePtr->Num()), AllGood);
    }
    return FReply::Handled();
}


TArray<FTransform> STransformHelperWindow::GetSelectionActorsTransforms()
{
    TArray<FTransform> list;
    auto LevelEditor = GetWorldSelectionSet();
    TArray<UObject*> SelectedObjects = LevelEditor? LevelEditor->GetSelectedObjects(AActor::StaticClass()) : TArray<UObject*>();

    for (auto& object : SelectedObjects)
    {
        AActor* AsActor = Cast<AActor>(object);
        if (AsActor)
        {
            FTransform newTransform = AsActor->GetActorTransform();
            list.Add(newTransform);
        }
    }

    return list;
}


UTypedElementSelectionSet* STransformHelperWindow::GetWorldSelectionSet()
{
   
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();
    if (LevelEditor.IsValid())
    {
        return LevelEditor->GetMutableElementSelectionSet();
    }

    return nullptr;

}

void STransformHelperWindow::ShowMessage(FString message, HelperState state)
{
    FColor Bad = FColor(255, 0, 0, 255);
    FColor Good = FColor(0, 255, 0, 255);
    FColor UsedColor = state == HelperState::AllGood ? Good : Bad;

    ResultsBorder->SetBorderBackgroundColor(UsedColor);
    ResultsText->SetText(FText::FromString(message));
    ResultsText->SetColorAndOpacity(UsedColor);
}

FString STransformHelperWindow::ParseTransformString(FTransform transform)
{
    FString location = FString::Printf(TEXT("Location : ( X : %f , Y: %f , Z: %f )\n"), transform.GetLocation().X, transform.GetLocation().Y, transform.GetLocation().Z);
    FString rotation = FString::Printf(TEXT("Rotation : ( Roll : %f , Yaw: %f , Pitch: %f )\n"), transform.GetRotation().Rotator().Roll, transform.GetRotation().Rotator().Yaw, transform.GetRotation().Rotator().Pitch);
    FString scale = FString::Printf(TEXT("Scale : ( X : %f , Y: %f , Z: %f )"), transform.GetScale3D().X, transform.GetScale3D().Y, transform.GetScale3D().Z);

    FString results = location + rotation + scale;
    return results;
}
    
void SAboutWindow::Construct(const FArguments& args)
{
    const FSlateBrush* AboutLogo = FAMS_PluginEditor::Get().AMS_StyleSet->GetBrush(FName("AboutWindow"));

    FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
    FontInfo.Size = 8.f;

    FSlateFontInfo copyRights = FontInfo;
    copyRights.Size = 6.f;

    ChildSlot
    [
         SNew(SBorder)
        .BorderBackgroundColor(FColor(192, 192, 192, 255))
        .Padding(0.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            [
                SNew(SImage)
                .Image(AboutLogo)
            ]
            +SVerticalBox::Slot()
            .Padding(5.0f)
            [
               SNew(SVerticalBox)
               + SVerticalBox::Slot()
               .Padding(0.0f,5.0f,0.0f,0.0f)
               [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("Arrows Interactive Systems (AIS) is a small sudanese indie games development studio.\nthis plugin was made for inhouse projects and distrebuted to help other developers too")))
                    .Font(FontInfo)
                    .AutoWrapText(true)
               ]
               + SVerticalBox::Slot()
               .VAlign(VAlign_Bottom)
               [
                   SNew(SHorizontalBox)
                   +SHorizontalBox::Slot()
                   .HAlign(HAlign_Left)
                   [
                       SNew(STextBlock)
                       .Text(FText::FromString(TEXT("Arrows Missions System v0.1")))
                      .Font(copyRights)
                   ]
                  + SHorizontalBox::Slot()
                   .HAlign(HAlign_Right)
                   [
                       SNew(STextBlock)
                       .Text(FText::FromString(TEXT("All Rights Are Reserved AIS Studio @2023")))
                        .Font(copyRights)
                   ]
                   
               ]

            ]
        ]
    ];
}