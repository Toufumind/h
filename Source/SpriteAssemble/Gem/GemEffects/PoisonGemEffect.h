#pragma once

#include "../GemEffect.h"
#include "PoisonGemEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class SPRITEASSEMBLE_API UPoisonGemEffect : public UGemEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	FGameplayTag PoisonTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float DamagePerSecondMultiplier = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 StackCount = 1;

	virtual void HandleShotHitSpec(const FShotHitSpec& HitSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const override;
};
