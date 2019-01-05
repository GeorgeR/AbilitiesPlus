// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AbilitiesPlusTargetType.h"

#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilityTypes.h"

void UAPTargetType::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const
{
	return;
}

void UAPTargetType_UseOwner::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const
{
	auto Actor = Cast<AActor>(TargetingCharacter);
	Actors.Add(Actor);
}

void UAPTargetType_UseEventData::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
		HitResults.Add(*FoundHitResult);
	else if (EventData.Target)
		Actors.Add(const_cast<AActor*>(EventData.Target));
}
