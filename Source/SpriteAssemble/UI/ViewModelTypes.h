#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "../Run/RunState.h"
#include "ViewModelTypes.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FHealthViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRewardOptionViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 OptionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	FRewardSpec RewardSpec;
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRunViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ERunPhase CurrentPhase = ERunPhase::None;

	UPROPERTY(BlueprintReadOnly)
	int32 RedCore = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 CompletedNodeCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 GemSlotCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PotionMax = 0;
};
