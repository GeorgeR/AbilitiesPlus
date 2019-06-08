#include "AbilitiesPlusAttributeSet.h"

#include "GameFramework/Character.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAbilitiesPlusAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// #todo Check if Attribute is Max, and adjust base for this change
}

void UAbilitiesPlusAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	auto Context = Data.EffectSpec.GetContext();
	auto Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const auto& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue = 0.0f;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
		DeltaValue = Data.EvaluatedData.Magnitude;

	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ACharacter* TargetCharacter = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacter>(TargetActor);
	}

	auto& Attribute = Data.EvaluatedData.Attribute;
	PostAttributeChange(Attribute);
}

void UAbilitiesPlusAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	auto AbilitySystemComponent = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystemComponent)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.0f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UAbilitiesPlusAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, const FGameplayAttribute* MaxAttribute /*= nullptr*/)
{
	auto Value = Attribute.GetNumericValue(this);
	if (MaxAttribute != nullptr)
	{
		auto MaxValue = MaxAttribute->GetNumericValue(this);
		Value = FMath::Clamp(Value, 0.0f, MaxValue);
	}

	Attribute.SetNumericValueChecked(Value, this);

	if (Value <= SMALL_NUMBER)
	{
		auto TagName = FName(*FString::Printf(TEXT("Event.Depleted.%s"), *Attribute.GetName()));
		auto Tag = FGameplayTag::RequestGameplayTag(TagName);
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetActorInfo()->OwnerActor.Get(), Tag, MoveTemp(Payload));
	}
}
