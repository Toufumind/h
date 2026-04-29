#pragma once

#include "../GemEffect.h"
#include "SplitGemEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class SPRITEASSEMBLE_API USplitGemEffect : public UGemEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 ExtraShotCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float SpreadAngle = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float SecondaryDamageMultiplier = 0.5f;

	virtual void ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const override;
};
