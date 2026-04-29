#include "TripleShotGemEffect.h"

void UTripleShotGemEffect::ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	ShotSpawnSpec.ShotCount *= FMath::Max(1, ShotCount);
	ShotSpawnSpec.SpreadAngle = FMath::Max(ShotSpawnSpec.SpreadAngle, SpreadAngle);
	ShotSpawnSpec.DamageSpec.FinalDamage *= DamageMultiplier;
}
