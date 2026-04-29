#include "EffectComponent.h"
#include "../Combat/HealthComponent.h"

UEffectComponent::UEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FGameplayCommandResult UEffectComponent::ApplyStatusEffect(const FStatusEffectSpec& StatusEffectSpec)
{
	if (!StatusEffectSpec.EffectTag.IsValid())
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "InvalidStatusEffectTag", "Status effect tag is invalid."));
	}

	const int32 StackCount = FMath::Max(1, StatusEffectSpec.StackCount);
	for (FActiveStatusEffect& ActiveStatusEffect : ActiveStatusEffects)
	{
		if (ActiveStatusEffect.Spec.EffectTag == StatusEffectSpec.EffectTag)
		{
			ActiveStatusEffect.Spec = StatusEffectSpec;
			ActiveStatusEffect.CurrentStacks = FMath::Max(ActiveStatusEffect.CurrentStacks, StackCount);
			ActiveStatusEffect.RemainingDuration = FMath::Max(0.0f, StatusEffectSpec.Duration);
			OnStatusEffectApplied.Broadcast(this, ActiveStatusEffect.Spec);
			return FGameplayCommandResult::Success();
		}
	}

	FActiveStatusEffect ActiveStatusEffect;
	ActiveStatusEffect.Spec = StatusEffectSpec;
	ActiveStatusEffect.CurrentStacks = StackCount;
	ActiveStatusEffect.RemainingDuration = FMath::Max(0.0f, StatusEffectSpec.Duration);
	ActiveStatusEffect.TickAccumulator = 0.0f;
	ActiveStatusEffects.Add(ActiveStatusEffect);

	OnStatusEffectApplied.Broadcast(this, StatusEffectSpec);
	return FGameplayCommandResult::Success();
}

bool UEffectComponent::HasStatusEffect(FGameplayTag EffectTag) const
{
	return ActiveStatusEffects.ContainsByPredicate([EffectTag](const FActiveStatusEffect& ActiveStatusEffect)
	{
		return ActiveStatusEffect.Spec.EffectTag == EffectTag;
	});
}

void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 Index = ActiveStatusEffects.Num() - 1; Index >= 0; --Index)
	{
		FActiveStatusEffect& ActiveStatusEffect = ActiveStatusEffects[Index];
		TickStatusEffect(ActiveStatusEffect, DeltaTime);
		if (ActiveStatusEffect.RemainingDuration <= 0.0f)
		{
			ExpireStatusEffectAt(Index);
		}
	}
}

void UEffectComponent::TickStatusEffect(FActiveStatusEffect& ActiveStatusEffect, float DeltaTime)
{
	ActiveStatusEffect.RemainingDuration -= DeltaTime;
	ActiveStatusEffect.TickAccumulator += DeltaTime;

	while (ActiveStatusEffect.TickAccumulator >= StatusTickInterval)
	{
		ActiveStatusEffect.TickAccumulator -= StatusTickInterval;
		ApplyPeriodicDamage(ActiveStatusEffect);
	}
}

void UEffectComponent::ApplyPeriodicDamage(const FActiveStatusEffect& ActiveStatusEffect)
{
	if (ActiveStatusEffect.Spec.Magnitude <= 0.0f)
	{
		return;
	}

	UHealthComponent* HealthComponent = GetHealthComponent();
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return;
	}

	FDamageSpec DamageSpec;
	DamageSpec.SourceActor = ActiveStatusEffect.Spec.SourceActor;
	DamageSpec.InstigatorActor = ActiveStatusEffect.Spec.SourceActor;
	DamageSpec.Team = EGameplayTeam::Neutral;
	DamageSpec.BaseDamage = ActiveStatusEffect.Spec.Magnitude * ActiveStatusEffect.CurrentStacks;
	DamageSpec.FinalDamage = DamageSpec.BaseDamage;
	HealthComponent->ApplyDamageSpec(DamageSpec);
}

void UEffectComponent::ExpireStatusEffectAt(int32 Index)
{
	if (!ActiveStatusEffects.IsValidIndex(Index))
	{
		return;
	}

	const FStatusEffectSpec ExpiredSpec = ActiveStatusEffects[Index].Spec;
	ActiveStatusEffects.RemoveAtSwap(Index);
	OnStatusEffectExpired.Broadcast(this, ExpiredSpec);
}

UHealthComponent* UEffectComponent::GetHealthComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr;
}
