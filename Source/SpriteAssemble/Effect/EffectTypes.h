#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "EffectTypes.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FActiveStatusEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FStatusEffectSpec Spec;

	UPROPERTY(BlueprintReadOnly)
	float RemainingDuration = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float TickAccumulator = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentStacks = 1;
};
