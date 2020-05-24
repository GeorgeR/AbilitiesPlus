// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitiesPlusBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectContainer.h"
#include "Engine/EngineTypes.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "AbilitySystemGlobals.h"
#include "AbilitiesPlusTargetType.h"

FGameplayEffectContainerSpec UAbilitiesPlusBlueprintLibrary::MakeEffectContainerSpecFromContainer(UGameplayAbility* Ability, const FGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	// First figure out our actor info
	FGameplayEffectContainerSpec ReturnSpec;
	auto OwningActor = Ability->GetOwningActorFromActorInfo();

	auto OwningCharacter = Cast<ACharacter>(OwningActor);
	ensure(OwningCharacter->Implements<UAbilitySystemInterface>());

	auto OwningAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);
	if (OwningAbilitySystemComponent)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UAPTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			auto AvatarActor = Ability->GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		//if (OverrideGameplayLevel == INDEX_NONE)
		//	OverrideGameplayLevel = OwningAbilitySystemComponent->GetDefaultAbilityLevel();

		// Build GameplayEffectSpecs for each applied effect
		for (const auto& EffectClass : Container.TargetGameplayEffectClasses)
			ReturnSpec.TargetGameplayEffectSpecs.Add(Ability->MakeOutgoingGameplayEffectSpec(EffectClass));
	}

	return ReturnSpec;
}

FGameplayEffectContainerSpec UAbilitiesPlusBlueprintLibrary::MakeEffectContainerSpec(UGameplayAbility* Ability, const TMap<FGameplayTag, FGameplayEffectContainer>& EffectContainerMap, FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	auto FoundContainer = EffectContainerMap.Find(ContainerTag);
	if (FoundContainer)
		return MakeEffectContainerSpecFromContainer(Ability, *FoundContainer, EventData);

	return FGameplayEffectContainerSpec();
}
