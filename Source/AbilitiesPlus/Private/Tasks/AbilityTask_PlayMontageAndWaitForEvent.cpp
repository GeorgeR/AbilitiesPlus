// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Tasks/AbilityTask_PlayMontageAndWaitForEvent.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"

UAbilityTask_PlayMontageAndWaitForEvent::UAbilityTask_PlayMontageAndWaitForEvent()
	: Rate(1.0f),
	bStopWhenAbilityEnds(true) { }

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			auto Character = Cast<ACharacter>(GetAvatarActor());
#if ENGINE_MINOR_VERSION >= 24
			const auto CharacterRole = Character->GetLocalRole();
#else
            const auto CharacterRole = Character->Role;
#endif
			if (Character && (CharacterRole == ROLE_Authority || (CharacterRole == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bInterrupted)
		if (ShouldBroadcastAbilityTaskDelegates())
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		else
			if (ShouldBroadcastAbilityTaskDelegates())
				OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	// #todo Merge this fix back to engine, it was calling the wrong callback
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
		if (ShouldBroadcastAbilityTaskDelegates())
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());

	EndTask();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
        auto TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UAbilityTask_PlayMontageAndWaitForEvent* UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	auto Result = NewAbilityTask<UAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	Result->MontageToPlay = MontageToPlay;
	Result->EventTags = EventTags;
	Result->Rate = Rate;
	Result->StartSection = StartSection;
	Result->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	Result->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return Result;
}

void UAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
		return;

    auto bPlayedMontage = false;

#pragma warning(push)
#pragma warning(disable: 4458)
	auto AbilitySystemComponent = GetTargetAbilitySystemComponent<UAbilitySystemComponent>();
#pragma warning(pop)

	if (AbilitySystemComponent)
	{
		const auto ActorInfo = Ability->GetCurrentActorInfo();
		auto AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// Bind to event callback
			EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));

			if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
					return;

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				auto Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);

				bPlayedMontage = true;
			}
		}
		else
			ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
	}
	else
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}

	SetWaitingOnAvatar();
}

void UAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
			StopPlayingMontage();
	}

#pragma warning(push)
#pragma warning(disable: 4458)
	auto AbilitySystemComponent = GetTargetAbilitySystemComponent<UAbilitySystemComponent>();
#pragma warning(pop)
	if (AbilitySystemComponent)
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);

	Super::OnDestroy(AbilityEnded);
}

bool UAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const auto ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
		return false;

    const auto AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
		return false;

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			auto MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const auto ActorInfo = Ability->GetCurrentActorInfo();
        const auto AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

template <>
UAbilitySystemComponent* UAbilityTask_PlayMontageAndWaitForEvent::GetTargetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}
