#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplaySpecs.h"
#include "GameplayCommand.generated.h"

UENUM(BlueprintType)
enum class EGameplayCommandType : uint8
{
	None,
	Heal,
	ApplyDamage,
	ApplyStatusEffect,
	RemoveStatusEffect,
	CreateAreaEffect,
	AddAttributeModifier,
	RemoveAttributeModifier,
	AddGem,
	UpgradeGem,
	EquipGem,
	ReorderGems,
	AddRedCore,
	SpendRedCore,
	AddGemSlot,
	AddPotionMax,
	RefillPotions,
	ReviveOnce,
	StartEncounter,
	CompleteEncounter,
	BuildRewardOptions,
	ClaimReward,
	SelectNode,
	CompleteNode
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FGameplayCommand
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EGameplayCommandType Type = EGameplayCommandType::None;

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
	TObjectPtr<UObject> ObjectValue = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGuid GuidValue;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGuid> GuidArrayValue;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadWrite)
	FDamageSpec DamageSpec;

	UPROPERTY(BlueprintReadWrite)
	FStatusEffectSpec StatusEffectSpec;

	UPROPERTY(BlueprintReadWrite)
	FAreaEffectSpec AreaEffectSpec;

	UPROPERTY(BlueprintReadWrite)
	FRewardSpec RewardSpec;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FGameplayCommandResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHandled = false;

	UPROPERTY(BlueprintReadOnly)
	bool bSucceeded = false;

	UPROPERTY(BlueprintReadOnly)
	FText FailureReason;

	static FGameplayCommandResult Unhandled()
	{
		return FGameplayCommandResult();
	}

	static FGameplayCommandResult Success()
	{
		FGameplayCommandResult Result;
		Result.bHandled = true;
		Result.bSucceeded = true;
		return Result;
	}

	static FGameplayCommandResult Failure(const FText& Reason)
	{
		FGameplayCommandResult Result;
		Result.bHandled = true;
		Result.bSucceeded = false;
		Result.FailureReason = Reason;
		return Result;
	}
};
