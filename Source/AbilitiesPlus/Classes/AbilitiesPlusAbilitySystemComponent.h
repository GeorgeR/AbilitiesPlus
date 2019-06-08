#pragma once

#include "AbilitySystemComponent.h"

#include "AbilitiesPlusAbilitySystemComponent.generated.h"

class IAPPlayerInterface;

/**
 * Subclass of ability system component with game-specific data
 * Most games will need to make a game-specific subclass to provide utility functions
 */
UCLASS()
class ABILITIESPLUS_API UAPAbilitySystemComponent
	: public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void AddInitialAbilitiesAndEffects(const TEnumAsByte<ENetRole> NetRole, const TScriptInterface<IAPPlayerInterface>& Character);
	virtual void RemoveInitialAbilitiesAndEffects(const TEnumAsByte<ENetRole> NetRole, const TScriptInterface<IAPPlayerInterface>& Character);

	/** Returns a list of currently active ability instances that match the tags */
	virtual void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities);

	/** Returns the default level used for ability activations, derived from the character */
	virtual int32 GetDefaultAbilityLevel() const;
};
