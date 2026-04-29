#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplaySpecs.h"
#include "RewardTypes.generated.h"

UENUM(BlueprintType)
enum class ERewardApplyType : uint8
{
	None,
	AddRedCore,
	AddGem,
	AddGemSlot,
	AddPotionMax
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRewardOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid OptionId;

	UPROPERTY(BlueprintReadOnly)
	FRewardSpec RewardSpec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> PayloadObject = nullptr;

	UPROPERTY(BlueprintReadOnly)
	ERewardApplyType ApplyType = ERewardApplyType::None;
};
