#include "CombatComponent.h"
#include "DamageResolver.h"
#include "../Core/GameplayInterfaces.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FAttackSpec UCombatComponent::BuildAttackSpec(AActor* TargetActor, float BaseDamage, float Range, const FVector& Direction) const
{
	FAttackSpec AttackSpec;
	AttackSpec.SourceActor = GetOwner();
	AttackSpec.InstigatorActor = GetOwner();
	AttackSpec.TargetActor = TargetActor;
	AttackSpec.BaseDamage = BaseDamage;
	AttackSpec.Range = Range;
	AttackSpec.Direction = Direction.GetSafeNormal();
	return AttackSpec;
}

FDamageSpec UCombatComponent::BuildDamageSpec(const FAttackSpec& AttackSpec) const
{
	FDamageSpec DamageSpec;
	DamageSpec.SourceActor = AttackSpec.SourceActor;
	DamageSpec.InstigatorActor = AttackSpec.InstigatorActor;
	DamageSpec.Team = AttackSpec.Team;
	DamageSpec.BaseDamage = AttackSpec.BaseDamage;
	DamageSpec.FinalDamage = AttackSpec.BaseDamage;
	DamageSpec.Tags = AttackSpec.Tags;
	return DamageSpec;
}

FGameplayCommandResult UCombatComponent::ResolveShotHit(FShotHitSpec& HitSpec)
{
	FGameplayCommandResult Result = FDamageResolver::ApplyDamage(HitSpec.TargetActor, HitSpec.DamageSpec);
	if (!Result.bHandled && HitSpec.TargetActor && HitSpec.TargetActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		Result = IDamageable::Execute_ApplyDamageSpec(HitSpec.TargetActor, HitSpec.DamageSpec);
	}

	if (Result.bHandled && Result.bSucceeded)
	{
		HitSpec.AppliedDamage = HitSpec.DamageSpec.FinalDamage;
		OnHitConfirmed.Broadcast(this, HitSpec);
	}

	return Result;
}
