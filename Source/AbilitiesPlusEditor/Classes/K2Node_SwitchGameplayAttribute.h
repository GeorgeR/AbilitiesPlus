#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_Switch.h"
#include "AttributeSet.h"

#include "K2Node_SwitchGameplayAttribute.generated.h"

class FBlueprintActionDatabaseRegistrar;

UCLASS(MinimalAPI)
class UK2Node_SwitchGameplayAttribute
    : public UK2Node_Switch
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=PinOptions)
	TArray<FGameplayAttribute> PinAttributes;

	UPROPERTY()
	TArray<FName> PinNames;

    UK2Node_SwitchGameplayAttribute();
	
	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	// End of UObject interface

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void PostPlacedNewNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface

	// UK2Node_Switch Interface
	ABILITIESPLUSEDITOR_API virtual void AddPinToSwitchNode() override;
	ABILITIESPLUSEDITOR_API virtual FEdGraphPinType GetInnerCaseType() const override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	FName GetPinNameGivenIndex(int32 Index) const override;

	//void SetGameplayAttribute(const FGameplayAttribute& InAttribute);

protected:
	virtual void CreateFunctionPin() override;
	virtual void CreateCasePins() override;
	virtual void CreateSelectionPin() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
};
