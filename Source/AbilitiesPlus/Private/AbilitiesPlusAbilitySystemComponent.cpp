#include "AbilitiesPlusAbilitySystemComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "AbilitiesPlusPlayerInterface.h"

void UAPAbilitySystemComponent::AddInitialAbilitiesAndEffects(const TEnumAsByte<ENetRole> NetRole, const TScriptInterface<IAPPlayerInterface>& Player)
{
	if (NetRole != ENetRole::ROLE_Authority)
		return;

	TArray<TSoftClassPtr<UGameplayAbility>> Abilities;
	if (IAPPlayerInterface::Execute_GetInitialAbilities(Player.GetObject(), Abilities))
		for (auto& Ability : Abilities)
			GiveAbility(FGameplayAbilitySpec(Ability.LoadSynchronous(), 1, INDEX_NONE, this));

	TArray<TSoftClassPtr<UGameplayEffect>> Effects;
	if (IAPPlayerInterface::Execute_GetInitialEffects(Player.GetObject(), Effects))
		for (auto& Effect : Effects)
		{
			auto EffectContext = MakeEffectContext();
			EffectContext.AddSourceObject(Player.GetObject());

			auto EffectHandle = MakeOutgoingSpec(Effect.LoadSynchronous(), 1.0f, EffectContext);
			if (EffectHandle.IsValid())
				auto ActiveHandle = ApplyGameplayEffectSpecToTarget(*EffectHandle.Data.Get(), this);
		}
}

void UAPAbilitySystemComponent::RemoveInitialAbilitiesAndEffects(const TEnumAsByte<ENetRole> NetRole, const TScriptInterface<IAPPlayerInterface>& Player)
{
	if (NetRole != ENetRole::ROLE_Authority)
		return;

	TArray<TSoftClassPtr<UGameplayAbility>> Abilities;
	if (Player->GetInitialAbilities(Abilities))
	{
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const auto& Spec : GetActivatableAbilities())
			if ((Spec.SourceObject == Player.GetObject()) && Abilities.Contains(Spec.Ability->GetClass()))
				AbilitiesToRemove.Add(Spec.Handle);

		for (auto& Ability : AbilitiesToRemove)
			ClearAbility(Ability);
	}

	FGameplayEffectQuery Query;
	Query.EffectSource = this;
	RemoveActiveEffects(Query);
}

void UAPAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (auto Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		auto AbilityInstances = Spec->GetAbilityInstances();
		for (auto ActiveAbility : AbilityInstances)
			ActiveAbilities.Add(Cast<UGameplayAbility>(ActiveAbility));
	}
}

int32 UAPAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	auto OwningPlayer = Cast<IAPPlayerInterface>(OwnerActor);
	if (OwningPlayer)
		OwningPlayer->GetLevel();

	return 1;
}
