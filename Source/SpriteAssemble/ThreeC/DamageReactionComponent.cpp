#include "DamageReactionComponent.h"

#include "../Combat/HealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UDamageReactionComponent::UDamageReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthComponent* HealthComponent = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr)
	{
		HealthComponent->OnDamaged.AddDynamic(this, &UDamageReactionComponent::HandleDamaged);
		HealthComponent->OnDeath.AddDynamic(this, &UDamageReactionComponent::HandleDeath);
	}
}

bool UDamageReactionComponent::CanReceiveDamage() const
{
	return !bDead && !bInvincible;
}

void UDamageReactionComponent::ResetReactionState()
{
	bDead = false;
	bInvincible = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
	}
	OnInvincibilityChanged.Broadcast(bInvincible);
}

void UDamageReactionComponent::HandleDamaged(UHealthComponent* HealthComponent, const FDamageSpec& DamageSpec)
{
	if (bDead)
	{
		return;
	}

	ApplyKnockback(DamageSpec);
	BeginInvincibility();
	OnDamageReaction.Broadcast(DamageSpec);
}

void UDamageReactionComponent::HandleDeath(UHealthComponent* HealthComponent)
{
	bDead = true;
	bInvincible = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
	}

	OnInvincibilityChanged.Broadcast(bInvincible);
	OnDeathReaction.Broadcast();
}

void UDamageReactionComponent::BeginInvincibility()
{
	bInvincible = true;
	OnInvincibilityChanged.Broadcast(bInvincible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &UDamageReactionComponent::EndInvincibility, InvincibilitySeconds, false);
	}
}

void UDamageReactionComponent::EndInvincibility()
{
	bInvincible = false;
	OnInvincibilityChanged.Broadcast(bInvincible);
}

void UDamageReactionComponent::ApplyKnockback(const FDamageSpec& DamageSpec)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Character->GetCharacterMovement())
	{
		return;
	}

	const AActor* SourceActor = DamageSpec.SourceActor.Get();
	FVector Direction = SourceActor ? (Character->GetActorLocation() - SourceActor->GetActorLocation()) : Character->GetActorForwardVector();
	Direction.Y = 0.0f;
	if (Direction.IsNearlyZero())
	{
		Direction = -Character->GetActorForwardVector();
	}
	Direction = Direction.GetSafeNormal();

	const float KnockbackStrength = DamageSpec.Knockback > 0.0f ? DamageSpec.Knockback : DefaultKnockbackStrength;
	const FVector LaunchVelocity = Direction * KnockbackStrength + FVector(0.0f, 0.0f, KnockbackUpwardStrength);
	Character->LaunchCharacter(LaunchVelocity, true, true);
}
