#include "IceMirrorComponent.h"
#include "IceMirrorDefinition.h"
#include "TargetingComponent.h"
#include "../Combat/CombatComponent.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../SpellCircuit/SpellCircuitComponent.h"
#include "../Shot/ShotWorldSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

UIceMirrorComponent::UIceMirrorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UIceMirrorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		StartAutoCast();
	}
}

void UIceMirrorComponent::StartAutoCast()
{
	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(AutoCastTimerHandle, this, &UIceMirrorComponent::TryCastOnce, GetAttackInterval(), true);
}

void UIceMirrorComponent::StopAutoCast()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoCastTimerHandle);
	}
}

void UIceMirrorComponent::TryCastOnce()
{
	UTargetingComponent* TargetingComponent = GetTargetingComponent();
	UCombatComponent* CombatComponent = GetCombatComponent();
	UShotWorldSubsystem* ShotSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UShotWorldSubsystem>() : nullptr;

	if (!TargetingComponent || !CombatComponent || !ShotSubsystem)
	{
		return;
	}

	const FTargetingResult TargetingResult = TargetingComponent->FindNearestTarget(GetRange(), Team);
	if (!TargetingResult.IsValid())
	{
		return;
	}

	FAttackSpec AttackSpec = CombatComponent->BuildAttackSpec(TargetingResult.TargetActor, GetBaseDamage(), GetRange(), TargetingResult.Direction);
	AttackSpec.Team = Team;

	TArray<FGameplayCommand> Commands;
	USpellCircuitComponent* SpellCircuitComponent = GetOwner() ? GetOwner()->FindComponentByClass<USpellCircuitComponent>() : nullptr;
	FSpellExecutionContext AttackContext;
	if (SpellCircuitComponent)
	{
		AttackContext = SpellCircuitComponent->MakeBaseContext(ESpellTriggerType::PreCast);
		SpellCircuitComponent->ModifyAttackSpec(AttackSpec, AttackContext, Commands);
	}

	FDamageSpec DamageSpec = CombatComponent->BuildDamageSpec(AttackSpec);
	if (SpellCircuitComponent)
	{
		FSpellExecutionContext DamageContext = SpellCircuitComponent->MakeBaseContext(ESpellTriggerType::OnBuildDamage);
		SpellCircuitComponent->ModifyDamageSpec(DamageSpec, DamageContext, Commands);
	}

	FShotSpawnSpec ShotSpawnSpec;
	ShotSpawnSpec.ShotCount = 1;
	ShotSpawnSpec.SpawnTransform = FTransform(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
	ShotSpawnSpec.Direction = TargetingResult.Direction;
	ShotSpawnSpec.Speed = GetShotSpeed();
	ShotSpawnSpec.Radius = GetShotRadius();
	ShotSpawnSpec.Lifetime = GetShotLifetime();
	ShotSpawnSpec.VisualType = GetShotVisualType();
	ShotSpawnSpec.OwnerActor = GetOwner();
	ShotSpawnSpec.InstigatorActor = GetOwner();
	ShotSpawnSpec.Team = Team;
	ShotSpawnSpec.DamageSpec = DamageSpec;

	if (SpellCircuitComponent)
	{
		FSpellExecutionContext ShotContext = SpellCircuitComponent->MakeBaseContext(ESpellTriggerType::OnBuildShot);
		SpellCircuitComponent->ModifyShotSpawnSpec(ShotSpawnSpec, ShotContext, Commands);
	}

	if (UGameplayCommandDispatcher* CommandDispatcher = GetWorld()->GetSubsystem<UGameplayCommandDispatcher>())
	{
		for (const FGameplayCommand& Command : Commands)
		{
			CommandDispatcher->QueueCommand(Command);
		}
	}

	ShotSubsystem->SubmitShotSpawnSpec(ShotSpawnSpec);
}

float UIceMirrorComponent::GetAttackInterval() const
{
	return Definition ? FMath::Max(0.05f, Definition->AttackInterval) : 0.5f;
}

float UIceMirrorComponent::GetRange() const
{
	return Definition ? Definition->Range : 900.0f;
}

float UIceMirrorComponent::GetBaseDamage() const
{
	return Definition ? Definition->BaseDamage : 20.0f;
}

float UIceMirrorComponent::GetShotSpeed() const
{
	return Definition ? Definition->ShotSpeed : 1000.0f;
}

float UIceMirrorComponent::GetShotRadius() const
{
	return Definition ? Definition->ShotRadius : 10.0f;
}

float UIceMirrorComponent::GetShotLifetime() const
{
	return Definition ? Definition->ShotLifetime : 1.5f;
}

FName UIceMirrorComponent::GetShotVisualType() const
{
	return Definition ? Definition->ShotVisualType : TEXT("Default");
}

UTargetingComponent* UIceMirrorComponent::GetTargetingComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UTargetingComponent>() : nullptr;
}

UCombatComponent* UIceMirrorComponent::GetCombatComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr;
}
