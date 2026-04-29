#include "CombatCommandWorldSubsystem.h"
#include "DamageResolver.h"
#include "HealthComponent.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "../Core/GameplayInterfaces.h"

void UCombatCommandWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		HealHandle = Dispatcher->RegisterHandler(EGameplayCommandType::Heal, FGameplayCommandNativeHandler::CreateUObject(this, &UCombatCommandWorldSubsystem::HandleHeal));
		ApplyDamageHandle = Dispatcher->RegisterHandler(EGameplayCommandType::ApplyDamage, FGameplayCommandNativeHandler::CreateUObject(this, &UCombatCommandWorldSubsystem::HandleApplyDamage));
	}
}

void UCombatCommandWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::Heal, HealHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::ApplyDamage, ApplyDamageHandle);
	}

	Super::Deinitialize();
}

FGameplayCommandResult UCombatCommandWorldSubsystem::HandleHeal(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.TargetActor.Get();
	if (!TargetActor)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "HealMissingTarget", "Heal target is missing."));
	}

	UHealthComponent* HealthComponent = TargetActor->FindComponentByClass<UHealthComponent>();
	if (!HealthComponent)
	{
		return FGameplayCommandResult::Unhandled();
	}

	FGameplayCommandResult Result = HealthComponent->Heal(Command.FloatValue);
	if (Result.bHandled && Result.bSucceeded)
	{
		if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
		{
			FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::Healed);
			Event.SourceActor = Command.SourceActor;
			Event.TargetActor = TargetActor;
			Event.FloatValue = Command.FloatValue;
			EventBus->QueueEvent(Event);
		}
	}

	return Result;
}

FGameplayCommandResult UCombatCommandWorldSubsystem::HandleApplyDamage(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.DamageSpec.InstigatorActor ? Command.TargetActor.Get() : Command.TargetActor.Get();
	FGameplayCommandResult Result = FDamageResolver::ApplyDamage(TargetActor, Command.DamageSpec);
	if (!Result.bHandled && TargetActor && TargetActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		Result = IDamageable::Execute_ApplyDamageSpec(TargetActor, Command.DamageSpec);
	}
	return Result;
}

UGameplayCommandDispatcher* UCombatCommandWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}
