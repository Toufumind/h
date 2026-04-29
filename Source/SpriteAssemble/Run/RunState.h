#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplayIds.h"
#include "../Core/GameplaySpecs.h"
#include "RunState.generated.h"

UENUM(BlueprintType)
enum class ERunPhase : uint8
{
	None,
	MapSelection,
	Combat,
	Reward,
	Preparation,
	RunWon,
	RunLost
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRunState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ERunPhase CurrentPhase = ERunPhase::MapSelection;

	UPROPERTY(BlueprintReadOnly)
	FRunNodeId CurrentNodeId;

	UPROPERTY(BlueprintReadOnly)
	int32 CompletedNodeCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 RedCore = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 GemSlotCount = 3;

	UPROPERTY(BlueprintReadOnly)
	int32 PotionMax = 1;

	UPROPERTY(BlueprintReadOnly)
	TArray<FRewardSpec> PendingRewardOptions;
};
