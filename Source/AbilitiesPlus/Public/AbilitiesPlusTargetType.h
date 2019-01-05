// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilityTypes.h"

#include "AbilitiesPlusTargetType.generated.h"

class AActor;
class ACharacter;

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin, DisplayName = "TargetType"))
class ABILITIESPLUS_API UAPTargetType
	: public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UAPTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable, meta = (DisplayName = "TargetType_UseOwner"))
class ABILITIESPLUS_API UAPTargetType_UseOwner
	: public UAPTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UAPTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable, meta = (DisplayName = "TargetType_UseEventData"))
class ABILITIESPLUS_API UAPTargetType_UseEventData
	: public UAPTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UAPTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& HitResults, TArray<AActor*>& Actors) const override;
};
