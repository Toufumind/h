#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "AreaEffectTypes.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FAreaEffectInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid InstanceId;

	UPROPERTY(BlueprintReadOnly)
	FAreaEffectSpec Spec;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float TickAccumulator = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bActive = false;

	UPROPERTY(BlueprintReadOnly)
	bool bAlive = false;
};
