#pragma once

#include "../GemEffect.h"
#include "PierceGemEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class SPRITEASSEMBLE_API UPierceGemEffect : public UGemEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	int32 PierceCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	float PostPierceDamageMultiplier = 0.6f;

	virtual void ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const override;
};
