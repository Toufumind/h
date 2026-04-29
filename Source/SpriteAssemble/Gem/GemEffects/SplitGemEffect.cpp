#include "SplitGemEffect.h"

void USplitGemEffect::ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	if (!Context.bAllowSplit)
	{
		return;
	}

	ShotSpawnSpec.ShotCount += FMath::Max(0, ExtraShotCount);
	ShotSpawnSpec.SpreadAngle = FMath::Max(ShotSpawnSpec.SpreadAngle, SpreadAngle);
	ShotSpawnSpec.NextGemIndex = Context.CurrentGemIndex + 1;
	ShotSpawnSpec.DamageSpec.FinalDamage *= SecondaryDamageMultiplier;

	Context.bAllowSplit = false;
}
