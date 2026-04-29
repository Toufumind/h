#pragma once

#include "CoreMinimal.h"
#include "../Core/GameplayIds.h"
#include "GemTypes.generated.h"

UENUM(BlueprintType)
enum class EGemRarity : uint8
{
	Common,
	Rare,
	Epic
};

UENUM(BlueprintType)
enum class EGemSlotEffectType : uint8
{
	Launch,
	Trajectory,
	Hit,
	Condition,
	Control,
	Terminal,
	Function
};

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FGemRuntimeInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGemRuntimeId RuntimeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UGemDefinition> Definition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EquippedSlotIndex = INDEX_NONE;

	bool IsValid() const { return RuntimeId.IsValid() && Definition != nullptr; }
};
