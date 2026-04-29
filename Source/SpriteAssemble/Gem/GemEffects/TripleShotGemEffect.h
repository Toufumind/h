#pragma once

#include "../GemEffect.h"
#include "TripleShotGemEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class SPRITEASSEMBLE_API UTripleShotGemEffect : public UGemEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 ShotCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float DamageMultiplier = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float SpreadAngle = 0.0f;

	virtual void ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const override;
};
