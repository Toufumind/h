#include "EffectCommandWorldSubsystem.h"
#include "EffectComponent.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"

void UEffectCommandWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		ApplyStatusEffectHandle = Dispatcher->RegisterHandler(
			EGameplayCommandType::ApplyStatusEffect,
			FGameplayCommandNativeHandler::CreateUObject(this, &UEffectCommandWorldSubsystem::HandleApplyStatusEffect));
	}
}

void UEffectCommandWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::ApplyStatusEffect, ApplyStatusEffectHandle);
	}

	Super::Deinitialize();
}

FGameplayCommandResult UEffectCommandWorldSubsystem::HandleApplyStatusEffect(const FGameplayCommand& Command) const
{
	AActor* TargetActor = Command.StatusEffectSpec.TargetActor ? Command.StatusEffectSpec.TargetActor.Get() : Command.TargetActor.Get();
	if (!TargetActor)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "StatusEffectMissingTarget", "Status effect target is missing."));
	}

	UEffectComponent* EffectComponent = TargetActor->FindComponentByClass<UEffectComponent>();
	if (!EffectComponent)
	{
		return FGameplayCommandResult::Unhandled();
	}

	FGameplayCommandResult Result = EffectComponent->ApplyStatusEffect(Command.StatusEffectSpec);
	if (Result.bHandled && Result.bSucceeded)
	{
		if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
		{
			FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::StatusApplied);
			Event.SourceActor = Command.SourceActor;
			Event.TargetActor = TargetActor;
			Event.NameValue = Command.StatusEffectSpec.EffectTag.GetTagName();
			EventBus->QueueEvent(Event);
		}
	}

	return Result;
}

UGameplayCommandDispatcher* UEffectCommandWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}
