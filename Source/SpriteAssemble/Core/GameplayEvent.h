#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplaySpecs.h"
#include "GameplayEvent.generated.h"

UENUM(BlueprintType)
enum class EGameplayEventType : uint8
{
	None,
	AttackStarted,
	ShotSpawned,
	ShotHit,
	Damaged,
	Healed,
	Death,
	GemTriggered,
	StatusApplied,
	AreaEffectCreated,
	RunCurrencyChanged,
	GemInventoryChanged,
	NodeSelected,
	NodeCompleted,
	EncounterCompleted,
	EncounterFailed,
	RewardOptionsBuilt,
	RewardClaimed,
	PreparationExited
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FGameplayEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EGameplayEventType Type = EGameplayEventType::None;

	UPROPERTY(BlueprintReadWrite)
	FGuid EventId;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float FloatValue = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	int32 IntValue = 0;

	UPROPERTY(BlueprintReadWrite)
	FName NameValue = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadWrite)
	FShotId ShotId;

	UPROPERTY(BlueprintReadWrite)
	FShotHitSpec ShotHitSpec;

	UPROPERTY(BlueprintReadWrite)
	FDamageSpec DamageSpec;

	UPROPERTY(BlueprintReadWrite)
	FRewardSpec RewardSpec;

	static FGameplayEvent Make(EGameplayEventType InType)
	{
		FGameplayEvent Event;
		Event.Type = InType;
		Event.EventId = FGuid::NewGuid();
		return Event;
	}
};
