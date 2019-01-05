#include "GameplayAbility_WithEffectContainer.h"

#include "AbilitiesPlusBlueprintLibrary.h"

TArray<FActiveGameplayEffectHandle> UGameplayAbility_WithEffectContainer::ApplyEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec, int32 OverrideGameplayLevel /*= -1*/)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const auto& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));

	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UGameplayAbility_WithEffectContainer::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	FGameplayEffectContainerSpec Spec = UAbilitiesPlusBlueprintLibrary::MakeEffectContainerSpec(this, this->EffectContainerMap, ContainerTag, EventData);
	return ApplyEffectContainerSpec(Spec);
}
