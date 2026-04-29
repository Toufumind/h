#include "AreaEffectWorldSubsystem.h"
#include "../Combat/CombatComponent.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "../Core/GameplayInterfaces.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

void UAreaEffectWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		CreateAreaEffectHandle = Dispatcher->RegisterHandler(
			EGameplayCommandType::CreateAreaEffect,
			FGameplayCommandNativeHandler::CreateUObject(this, &UAreaEffectWorldSubsystem::HandleCreateAreaEffect));
	}
}

void UAreaEffectWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::CreateAreaEffect, CreateAreaEffectHandle);
	}

	AreaEffects.Reset();
	Super::Deinitialize();
}

void UAreaEffectWorldSubsystem::Tick(float DeltaTime)
{
	for (FAreaEffectInstance& AreaEffect : AreaEffects)
	{
		if (AreaEffect.bAlive)
		{
			TickAreaEffect(AreaEffect, DeltaTime);
		}
	}

	RemoveDeadAreaEffects();
}

TStatId UAreaEffectWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAreaEffectWorldSubsystem, STATGROUP_Tickables);
}

FGuid UAreaEffectWorldSubsystem::CreateAreaEffect(const FAreaEffectSpec& AreaEffectSpec)
{
	FAreaEffectInstance AreaEffect;
	AreaEffect.InstanceId = FGuid::NewGuid();
	AreaEffect.Spec = AreaEffectSpec;
	AreaEffect.ElapsedTime = 0.0f;
	AreaEffect.TickAccumulator = 0.0f;
	AreaEffect.bActive = AreaEffectSpec.Delay <= 0.0f;
	AreaEffect.bAlive = true;

	const FGuid InstanceId = AreaEffect.InstanceId;
	AreaEffects.Add(AreaEffect);

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::AreaEffectCreated);
		Event.SourceActor = AreaEffectSpec.SourceActor;
		Event.NameValue = AreaEffectSpec.EffectTag.GetTagName();
		Event.FloatValue = AreaEffectSpec.Radius;
		EventBus->QueueEvent(Event);
	}

	return InstanceId;
}

FGameplayCommandResult UAreaEffectWorldSubsystem::HandleCreateAreaEffect(const FGameplayCommand& Command)
{
	CreateAreaEffect(Command.AreaEffectSpec);
	return FGameplayCommandResult::Success();
}

UGameplayCommandDispatcher* UAreaEffectWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}

void UAreaEffectWorldSubsystem::TickAreaEffect(FAreaEffectInstance& AreaEffect, float DeltaTime)
{
	AreaEffect.ElapsedTime += DeltaTime;

	if (!AreaEffect.bActive)
	{
		if (AreaEffect.ElapsedTime >= AreaEffect.Spec.Delay)
		{
			AreaEffect.bActive = true;
			AreaEffect.TickAccumulator = DamageTickInterval;
		}
		return;
	}

	const float ActiveElapsedTime = AreaEffect.ElapsedTime - FMath::Max(0.0f, AreaEffect.Spec.Delay);
	if (AreaEffect.Spec.Duration > 0.0f && ActiveElapsedTime >= AreaEffect.Spec.Duration)
	{
		AreaEffect.bAlive = false;
		return;
	}

	AreaEffect.TickAccumulator += DeltaTime;
	while (AreaEffect.TickAccumulator >= DamageTickInterval)
	{
		AreaEffect.TickAccumulator -= DamageTickInterval;
		ApplyAreaEffectTick(AreaEffect);
	}
}

void UAreaEffectWorldSubsystem::ApplyAreaEffectTick(const FAreaEffectInstance& AreaEffect)
{
	if (!GetWorld() || AreaEffect.Spec.Radius <= 0.0f)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AreaEffectWorldSubsystem), false);
	QueryParams.AddIgnoredActor(AreaEffect.Spec.SourceActor);

	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(AreaEffect.Spec.Radius);
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		AreaEffect.Spec.Location,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		CollisionShape,
		QueryParams);

	TSet<TWeakObjectPtr<AActor>> AffectedActors;
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* Candidate = OverlapResult.GetActor();
		if (!ShouldAffectActor(AreaEffect, Candidate) || AffectedActors.Contains(Candidate))
		{
			continue;
		}

		AffectedActors.Add(Candidate);

		FShotHitSpec HitSpec;
		HitSpec.SourceActor = AreaEffect.Spec.SourceActor;
		HitSpec.InstigatorActor = AreaEffect.Spec.SourceActor;
		HitSpec.TargetActor = Candidate;
		HitSpec.HitLocation = Candidate->GetActorLocation();
		HitSpec.DamageSpec = AreaEffect.Spec.DamageSpec;

		if (AActor* SourceActor = AreaEffect.Spec.SourceActor.Get())
		{
			if (UCombatComponent* CombatComponent = SourceActor->FindComponentByClass<UCombatComponent>())
			{
				CombatComponent->ResolveShotHit(HitSpec);
				continue;
			}
		}

		if (Candidate->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
		{
			IDamageable::Execute_ApplyDamageSpec(Candidate, AreaEffect.Spec.DamageSpec);
		}
	}
}

bool UAreaEffectWorldSubsystem::ShouldAffectActor(const FAreaEffectInstance& AreaEffect, AActor* Candidate) const
{
	if (!Candidate || Candidate == AreaEffect.Spec.SourceActor)
	{
		return false;
	}

	if (Candidate->GetClass()->ImplementsInterface(UTeamAgent::StaticClass()) &&
		ITeamAgent::Execute_GetGameplayTeam(Candidate) == AreaEffect.Spec.Team)
	{
		return false;
	}

	if (Candidate->GetClass()->ImplementsInterface(UTargetable::StaticClass()) &&
		!ITargetable::Execute_IsTargetable(Candidate))
	{
		return false;
	}

	return Candidate->GetClass()->ImplementsInterface(UDamageable::StaticClass());
}

void UAreaEffectWorldSubsystem::RemoveDeadAreaEffects()
{
	for (int32 Index = AreaEffects.Num() - 1; Index >= 0; --Index)
	{
		if (!AreaEffects[Index].bAlive)
		{
			AreaEffects.RemoveAtSwap(Index);
		}
	}
}
