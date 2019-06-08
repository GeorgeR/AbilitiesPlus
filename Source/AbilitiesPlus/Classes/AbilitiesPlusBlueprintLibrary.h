#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectContainer.h"
#include "GameplayAbilityTypes.h"

#include "AbilitiesPlusBlueprintLibrary.generated.h"

struct FActiveGameplayEffectHandle;
class UGameplayAbility;

UCLASS(BlueprintType)
class ABILITIESPLUS_API UAbilitiesPlusBlueprintLibrary
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Make gameplay effect container spec to be applied later, using the passed in container */
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	static FGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(UGameplayAbility* Ability, const FGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	static FGameplayEffectContainerSpec MakeEffectContainerSpec(UGameplayAbility* Ability, const TMap<FGameplayTag, FGameplayEffectContainer>& EffectContainerMap, FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
};
