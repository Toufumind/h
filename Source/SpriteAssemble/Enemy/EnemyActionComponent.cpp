#include "EnemyActionComponent.h"
#include "EnemyActionDefinition.h"
#include "../Core/GameplayInterfaces.h"
#include "../Weapon/ShooterComponent.h"
#include "GameFramework/Pawn.h"

UEnemyActionComponent::UEnemyActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyActionComponent::BeginPlay()
{
	Super::BeginPlay();
	BrainComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEnemyBrainComponent>() : nullptr;
	ShooterComponent = GetOwner() ? GetOwner()->FindComponentByClass<UShooterComponent>() : nullptr;
}

void UEnemyActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickActionPhase(DeltaTime);
	if (!BrainComponent)
	{
		return;
	}

	if (IsInAction())
	{
		return;
	}

	AActor* Target = BrainComponent->GetCurrentTarget();
	switch (BrainComponent->GetCurrentIntent())
	{
	case EEnemyIntent::Chase:
		ChaseTarget(Target, DeltaTime);
		break;
	case EEnemyIntent::Attack:
		TryBeginAttack(Target);
		break;
	default:
		break;
	}
}

void UEnemyActionComponent::Configure(float InMoveSpeed, float InContactDamage, float InAttackCooldown, EGameplayTeam InTeam, UEnemyActionDefinition* InDefaultAction)
{
	MoveSpeed = InMoveSpeed;
	ContactDamage = InContactDamage;
	AttackCooldown = InAttackCooldown;
	Team = InTeam;

	if (InDefaultAction)
	{
		TelegraphTime = InDefaultAction->TelegraphTime;
		StartupTime = InDefaultAction->StartupTime;
		ActiveTime = InDefaultAction->ActiveTime;
		RecoveryTime = InDefaultAction->RecoveryTime;
		AttackRange = InDefaultAction->Range;
		ContactDamage = InDefaultAction->Damage;
		bUseShot = InDefaultAction->bUseShot;
		ShotSpeed = InDefaultAction->ShotSpeed;
		ShotRadius = InDefaultAction->ShotRadius;
		ShotLifetime = InDefaultAction->ShotLifetime;
		ShotVisualType = InDefaultAction->ShotVisualType;
	}
}

void UEnemyActionComponent::ChaseTarget(AActor* Target, float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return;
	}

	const FVector Direction = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal2D();
	if (APawn* PawnOwner = Cast<APawn>(Owner))
	{
		PawnOwner->AddMovementInput(Direction, 1.0f);
	}
	else
	{
		Owner->AddActorWorldOffset(Direction * MoveSpeed * DeltaTime, true);
	}
}

void UEnemyActionComponent::TryBeginAttack(AActor* Target)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Target || CurrentPhase != EEnemyActionPhase::Ready)
	{
		return;
	}

	if (!Target->GetClass()->ImplementsInterface(UDamageable::StaticClass()) || !IsTargetInRange(Target))
	{
		return;
	}

	LockedTarget = Target;
	bHasAppliedActiveHit = false;
	EnterPhase(EEnemyActionPhase::Telegraph, TelegraphTime);
}

void UEnemyActionComponent::TickActionPhase(float DeltaTime)
{
	if (CurrentPhase == EEnemyActionPhase::Ready)
	{
		return;
	}

	PhaseRemainingTime -= DeltaTime;
	if (CurrentPhase == EEnemyActionPhase::Active && !bHasAppliedActiveHit)
	{
		ApplyActiveHit();
	}

	if (PhaseRemainingTime <= 0.0f)
	{
		AdvanceActionPhase();
	}
}

void UEnemyActionComponent::AdvanceActionPhase()
{
	switch (CurrentPhase)
	{
	case EEnemyActionPhase::Telegraph:
		EnterPhase(EEnemyActionPhase::Startup, StartupTime);
		break;
	case EEnemyActionPhase::Startup:
		EnterPhase(EEnemyActionPhase::Active, ActiveTime);
		break;
	case EEnemyActionPhase::Active:
		EnterPhase(EEnemyActionPhase::Recovery, RecoveryTime);
		break;
	case EEnemyActionPhase::Recovery:
		EnterPhase(EEnemyActionPhase::Cooldown, AttackCooldown);
		break;
	case EEnemyActionPhase::Cooldown:
	default:
		EnterPhase(EEnemyActionPhase::Ready, 0.0f);
		break;
	}
}

void UEnemyActionComponent::EnterPhase(EEnemyActionPhase NewPhase, float Duration)
{
	CurrentPhase = NewPhase;
	PhaseRemainingTime = FMath::Max(0.0f, Duration);

	if (CurrentPhase == EEnemyActionPhase::Active)
	{
		bHasAppliedActiveHit = false;
	}

	if (CurrentPhase == EEnemyActionPhase::Ready)
	{
		LockedTarget.Reset();
		bHasAppliedActiveHit = false;
	}
}

void UEnemyActionComponent::ApplyActiveHit()
{
	bHasAppliedActiveHit = true;

	AActor* Owner = GetOwner();
	AActor* Target = LockedTarget.Get();
	if (!Owner || !Target || !IsTargetInRange(Target))
	{
		return;
	}

	if (bUseShot)
	{
		FireShot(Target);
	}
	else
	{
		ApplyMeleeHit(Target);
	}
}

void UEnemyActionComponent::ApplyMeleeHit(AActor* Target)
{
	if (!Target || !Target->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return;
	}

	IDamageable::Execute_ApplyDamageSpec(Target, BuildDamageSpec());
}

void UEnemyActionComponent::FireShot(AActor* Target)
{
	if (!ShooterComponent)
	{
		ShooterComponent = GetOwner() ? GetOwner()->FindComponentByClass<UShooterComponent>() : nullptr;
	}

	if (ShooterComponent)
	{
		ShooterComponent->FireAtTarget(Target, BuildDamageSpec(), ShotSpeed, ShotRadius, ShotLifetime, ShotVisualType, Team);
	}
}

FDamageSpec UEnemyActionComponent::BuildDamageSpec() const
{
	AActor* Owner = GetOwner();

	FDamageSpec DamageSpec;
	DamageSpec.SourceActor = Owner;
	DamageSpec.InstigatorActor = Owner;
	DamageSpec.Team = Team;
	DamageSpec.BaseDamage = ContactDamage;
	DamageSpec.FinalDamage = ContactDamage;
	return DamageSpec;
}

bool UEnemyActionComponent::IsTargetInRange(AActor* Target) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return false;
	}

	return FVector::Dist2D(Owner->GetActorLocation(), Target->GetActorLocation()) <= AttackRange;
}
