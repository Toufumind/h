#include "PoisonGemEffect.h"

void UPoisonGemEffect::HandleShotHitSpec(const FShotHitSpec& HitSpec, FSpellExecutionContext& Context, TArray<FGameplayCommand>& OutCommands) const
{
	if (!HitSpec.TargetActor)
	{
		return;
	}

	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::ApplyStatusEffect;
	Command.SourceActor = HitSpec.SourceActor;
	Command.TargetActor = HitSpec.TargetActor;
	Command.StatusEffectSpec.EffectTag = PoisonTag;
	Command.StatusEffectSpec.SourceActor = HitSpec.SourceActor;
	Command.StatusEffectSpec.TargetActor = HitSpec.TargetActor;
	Command.StatusEffectSpec.Duration = Duration;
	Command.StatusEffectSpec.StackCount = FMath::Max(1, StackCount);
	Command.StatusEffectSpec.Magnitude = HitSpec.DamageSpec.BaseDamage * DamagePerSecondMultiplier;
	OutCommands.Add(Command);
}
