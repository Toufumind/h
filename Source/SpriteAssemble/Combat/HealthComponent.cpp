#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::InitializeHealth(float InMaxHealth)
{
	MaxHealth = FMath::Max(1.0f, InMaxHealth);
	CurrentHealth = MaxHealth;
	bIsDead = false;
	BroadcastHealthChanged();
}

FGameplayCommandResult UHealthComponent::ApplyDamageSpec(const FDamageSpec& DamageSpec)
{
	if (bIsDead)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "TargetAlreadyDead", "Target is already dead."));
	}

	const float DamageAmount = FMath::Max(0.0f, DamageSpec.FinalDamage);
	if (DamageAmount <= 0.0f)
	{
		return FGameplayCommandResult::Success();
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnDamaged.Broadcast(this, DamageSpec);
	BroadcastHealthChanged();

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return FGameplayCommandResult::Success();
}

FGameplayCommandResult UHealthComponent::Heal(float HealAmount)
{
	if (bIsDead)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "CannotHealDeadTarget", "Cannot heal a dead target."));
	}

	const float PositiveHealAmount = FMath::Max(0.0f, HealAmount);
	if (PositiveHealAmount <= 0.0f)
	{
		return FGameplayCommandResult::Success();
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + PositiveHealAmount, 0.0f, MaxHealth);
	BroadcastHealthChanged();
	return FGameplayCommandResult::Success();
}

void UHealthComponent::BroadcastHealthChanged()
{
	OnHealthChanged.Broadcast(this, CurrentHealth, MaxHealth);
}

void UHealthComponent::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	OnDeath.Broadcast(this);
}
