#include "ShooterComponent.h"
#include "../Shot/ShotWorldSubsystem.h"
#include "Engine/World.h"

UShooterComponent::UShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FShotSpawnSpec UShooterComponent::BuildShotSpawnSpec(AActor* TargetActor, const FDamageSpec& DamageSpec, float Speed, float Radius, float Lifetime, FName VisualType, EGameplayTeam Team) const
{
	AActor* Owner = GetOwner();
	const FVector SourceLocation = Owner ? Owner->GetActorLocation() : FVector::ZeroVector;
	const FVector TargetLocation = TargetActor ? TargetActor->GetActorLocation() : SourceLocation + FVector::ForwardVector;

	FShotSpawnSpec ShotSpawnSpec;
	ShotSpawnSpec.ShotCount = 1;
	ShotSpawnSpec.SpawnTransform = FTransform(SourceLocation);
	ShotSpawnSpec.Direction = (TargetLocation - SourceLocation).GetSafeNormal2D();
	ShotSpawnSpec.Speed = Speed;
	ShotSpawnSpec.Radius = Radius;
	ShotSpawnSpec.Lifetime = Lifetime;
	ShotSpawnSpec.VisualType = VisualType;
	ShotSpawnSpec.OwnerActor = Owner;
	ShotSpawnSpec.InstigatorActor = Owner;
	ShotSpawnSpec.Team = Team;
	ShotSpawnSpec.DamageSpec = DamageSpec;
	return ShotSpawnSpec;
}

TArray<FShotId> UShooterComponent::SubmitShotSpawnSpec(const FShotSpawnSpec& ShotSpawnSpec) const
{
	UShotWorldSubsystem* ShotSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UShotWorldSubsystem>() : nullptr;
	return ShotSubsystem ? ShotSubsystem->SubmitShotSpawnSpec(ShotSpawnSpec) : TArray<FShotId>();
}

TArray<FShotId> UShooterComponent::FireAtTarget(AActor* TargetActor, const FDamageSpec& DamageSpec, float Speed, float Radius, float Lifetime, FName VisualType, EGameplayTeam Team) const
{
	return SubmitShotSpawnSpec(BuildShotSpawnSpec(TargetActor, DamageSpec, Speed, Radius, Lifetime, VisualType, Team));
}
