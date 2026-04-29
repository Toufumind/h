#include "PierceGemEffect.h"

void UPierceGemEffect::ModifyShotSpawnSpec(FShotSpawnSpec& ShotSpawnSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	ShotSpawnSpec.PierceCount += FMath::Max(0, PierceCount);
	ShotSpawnSpec.PostPierceDamageMultiplier = FMath::Min(ShotSpawnSpec.PostPierceDamageMultiplier, PostPierceDamageMultiplier);
}
