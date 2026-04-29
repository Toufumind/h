#include "UICommandControllerComponent.h"
#include "../Core/GameplayCommand.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"

UUICommandControllerComponent::UUICommandControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUICommandControllerComponent::ClaimReward(int32 RewardOptionIndex)
{
	if (UGameplayCommandDispatcher* Dispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr)
	{
		FGameplayCommand Command;
		Command.Type = EGameplayCommandType::ClaimReward;
		Command.IntValue = RewardOptionIndex;
		Command.SourceActor = GetOwner();
		Dispatcher->QueueCommand(Command);
	}
}

void UUICommandControllerComponent::SelectNode(FName NodeId)
{
	if (UGameplayCommandDispatcher* Dispatcher = GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr)
	{
		FGameplayCommand Command;
		Command.Type = EGameplayCommandType::SelectNode;
		Command.NameValue = NodeId;
		Command.SourceActor = GetOwner();
		Dispatcher->QueueCommand(Command);
	}

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::NodeSelected);
		Event.NameValue = NodeId;
		Event.SourceActor = GetOwner();
		EventBus->QueueEvent(Event);
	}
}

void UUICommandControllerComponent::CompletePreparation(FName NodeId)
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::PreparationExited);
		Event.NameValue = NodeId;
		Event.SourceActor = GetOwner();
		EventBus->QueueEvent(Event);
	}
}
