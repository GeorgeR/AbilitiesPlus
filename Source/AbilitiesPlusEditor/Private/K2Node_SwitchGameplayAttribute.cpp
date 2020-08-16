#include "K2Node_SwitchGameplayAttribute.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "EdGraphSchema_K2.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_SwitchGameplayAttribute::UK2Node_SwitchGameplayAttribute()
{
	bHasDefaultPin = false; // there is no default value
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveNone;

    FunctionName = TEXT("NotEqual_GameplayAttributeGameplayAttribute");
    FunctionClass = UAbilitySystemBlueprintLibrary::StaticClass();
}

void UK2Node_SwitchGameplayAttribute::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    auto bIsDirty = false;
    const auto PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchGameplayAttribute, PinAttributes))
        bIsDirty = true;

    if (bIsDirty)
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UK2Node_SwitchGameplayAttribute::PostLoad()
{
	Super::PostLoad();
	if (auto* FunctionPin = FindPin(FunctionName))
        FunctionPin->DefaultObject = FunctionClass->GetDefaultObject();
}

FText UK2Node_SwitchGameplayAttribute::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return NSLOCTEXT("K2Node", "Switch_GameplayAttribute", "Switch on Gameplay Attribute");
}

FText UK2Node_SwitchGameplayAttribute::GetTooltipText() const
{
    return NSLOCTEXT("K2Node", "SwitchGameplayAttribute_ToolTip", "Selects an output that matches the input value");
}

void UK2Node_SwitchGameplayAttribute::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	// @todo: prompt to populate case nodes
}

void UK2Node_SwitchGameplayAttribute::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//struct GetMenuActions_Utils
	//{
	//	static void SetNodeStruct(UEdGraphNode* NewNode, FFieldVariant StructField, TWeakObjectPtr<UScriptStruct> NonConstPtr)
	//	{
	//		auto Hat = StructField;
 //           auto Node = CastChecked<UK2Node_SwitchGameplayAttribute>(NewNode);
	//		//Node->GameplayAttribute = NonConstPtr.Get();
	//	}
	//};

	//auto* NodeClass = GetClass();
	//ActionRegistrar.RegisterStructActions(FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateLambda([NodeClass](const UScriptStruct* InStruct) -> UBlueprintNodeSpawner*
	//{
 //       UBlueprintFieldNodeSpawner* NodeSpawner = nullptr;

	//	if(InStruct == FGameplayAttribute::StaticStruct())
 //       {
	//		NodeSpawner = UBlueprintFieldNodeSpawner::Create(NodeClass, const_cast<UScriptStruct*>(InStruct));
	//	    check(NodeSpawner != nullptr);

 //           const auto NonConstPtr = MakeWeakObjectPtr(const_cast<UScriptStruct*>(InStruct));
	//	    NodeSpawner->SetNodeFieldDelegate = UBlueprintFieldNodeSpawner::FSetNodeFieldDelegate::CreateStatic(GetMenuActions_Utils::SetNodeStruct, NonConstPtr);
 //       }
	//    
	//	return NodeSpawner;
	//}));

    const auto* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
        auto* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_SwitchGameplayAttribute::AddPinToSwitchNode()
{
    const auto PinName = GetUniquePinName();
	PinNames.Add(PinName);

	UEdGraphPin* NewPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
	NewPin->PinFriendlyName = FText::FromName(PinName);

	if (PinAttributes.Num() < PinNames.Num())
        PinAttributes.Add(FGameplayAttribute());
}

FEdGraphPinType UK2Node_SwitchGameplayAttribute::GetInnerCaseType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_String;
	return PinType;
}

FName UK2Node_SwitchGameplayAttribute::GetUniquePinName()
{
	FName NewPinName;
    auto Index = 0;
	while (true)
	{
		NewPinName = *FString::Printf(TEXT("Case_%d"), Index++);
		if (!FindPin(NewPinName))
            break;
    }
	return NewPinName;
}

FEdGraphPinType UK2Node_SwitchGameplayAttribute::GetPinType() const
{
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FGameplayAttribute::StaticStruct();
    return PinType;
}

FName UK2Node_SwitchGameplayAttribute::GetPinNameGivenIndex(int32 Index) const
{
	check(Index);
	return PinNames[Index];
}

void UK2Node_SwitchGameplayAttribute::CreateFunctionPin()
{
	auto* FunctionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, FunctionClass, FunctionName);
	FunctionPin->bDefaultValueIsReadOnly = true;
	FunctionPin->bNotConnectable = true;
	FunctionPin->bHidden = true;

	auto* Function = FindUField<UFunction>(FunctionClass, FunctionName);
	const bool bIsStaticFunc = Function->HasAllFunctionFlags(FUNC_Static);
	if (bIsStaticFunc)
	{
		// Wire up the self to the CDO of the class if it's not us
		if (auto* BP = GetBlueprint())
		{
			UClass* FunctionOwnerClass = Function->GetOuterUClass();
			if (!BP->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
                FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
        }
	}
}

void UK2Node_SwitchGameplayAttribute::CreateCasePins()
{
    while (PinAttributes.Num() > PinNames.Num())
	{
		const auto PinName = GetUniquePinName();
		PinNames.Add(PinName);
	}

	if (PinNames.Num() > PinAttributes.Num())
        PinNames.SetNum(PinAttributes.Num());

    for (auto Index = 0; Index < PinAttributes.Num(); ++Index)
  	{
		if (PinAttributes[Index].IsValid())
            PinNames[Index] = *PinAttributes[Index].GetName();
        else
            PinNames[Index] = GetUniquePinName();

        CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);
    }
}

void UK2Node_SwitchGameplayAttribute::CreateSelectionPin()
{
    auto* K2Schema = GetDefault<UEdGraphSchema_K2>();
    auto* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGameplayAttribute::StaticStruct(), TEXT("Selection"));
    K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
}

void UK2Node_SwitchGameplayAttribute::RemovePin(UEdGraphPin* TargetPin)
{
	checkSlow(TargetPin);

    const auto PinName = TargetPin->PinName;
    const auto Index = PinNames.IndexOfByKey(PinName);
	if (Index >= 0)
	{
		if (Index < PinAttributes.Num())
            PinAttributes.RemoveAt(Index);
        PinNames.RemoveAt(Index);
	}
}

#undef LOCTEXT_NAMESPACE
