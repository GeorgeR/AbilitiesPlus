#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PointerIsConvertibleFromTo.h"

#include "AbilitiesPlusPlayerInterface.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

UINTERFACE(MinimalAPI, BlueprintType)
class UAPPlayerInterface
	: public UInterface
{
	GENERATED_BODY()
};

// NOTE: For convenience, but not required.
class ABILITIESPLUS_API IAPPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Abilities Plus|Player", meta = (DisplayName = "GetAbilitySystemComponent"))
	UAbilitySystemComponent* GetAbilitySystemComponent_BP() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent_BP_Implementation() const = 0;

	template <class TAbilitySystemComponent, class = typename TEnableIf<TPointerIsConvertibleFromTo<TAbilitySystemComponent*, UAbilitySystemComponent>::Value>::Type>
	FORCEINLINE TAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return Cast<TAbilitySystemComponent>(GetAbilitySystemComponent_BP());
	}

	/* (Optional) gets the initial abilities to grant to this player, return false if there aren't any */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Abilities Plus|Player")
	bool GetInitialAbilities(TArray<TSoftClassPtr<UGameplayAbility>>& Abilities) const;
	virtual bool GetInitialAbilities_Implementation(TArray<TSoftClassPtr<UGameplayAbility>>& Abilities) const { return false; }

	/* (Optional) gets the initial effects to grant to this player */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Abilities Plus|Player")
	bool GetInitialEffects(TArray<TSoftClassPtr<UGameplayEffect>>& Effects) const;
	virtual bool GetInitialEffects_Implementation(TArray<TSoftClassPtr<UGameplayEffect>>& Effects) const { return false; }

	/** Returns the player level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Abilities Plus|Player")
	int32 GetLevel() const;
	virtual int32 GetLevel_Implementation() const { return 1; }

	/** Modifies the player level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Abilities Plus|Player")
	bool SetLevel(int32 Level);
	virtual bool SetLevel_Implementation(int32 Level) { return true; }
};
