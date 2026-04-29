#include "RunFlowWorldSubsystem.h"
#include "RunStateWorldSubsystem.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEventBus.h"

void URunFlowWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayEventBus* EventBus = GetEventBus())
	{
		NodeSelectedHandle = EventBus->Subscribe(EGameplayEventType::NodeSelected, FGameplayNativeEvent::FDelegate::CreateUObject(this, &URunFlowWorldSubsystem::HandleNodeSelected));
		EncounterCompletedHandle = EventBus->Subscribe(EGameplayEventType::EncounterCompleted, FGameplayNativeEvent::FDelegate::CreateUObject(this, &URunFlowWorldSubsystem::HandleEncounterCompleted));
		RewardClaimedHandle = EventBus->Subscribe(EGameplayEventType::RewardClaimed, FGameplayNativeEvent::FDelegate::CreateUObject(this, &URunFlowWorldSubsystem::HandleRewardClaimed));
		PreparationExitedHandle = EventBus->Subscribe(EGameplayEventType::PreparationExited, FGameplayNativeEvent::FDelegate::CreateUObject(this, &URunFlowWorldSubsystem::HandlePreparationExited));
	}
}

void URunFlowWorldSubsystem::Deinitialize()
{
	if (UGameplayEventBus* EventBus = GetEventBus())
	{
		EventBus->Unsubscribe(EGameplayEventType::NodeSelected, NodeSelectedHandle);
		EventBus->Unsubscribe(EGameplayEventType::EncounterCompleted, EncounterCompletedHandle);
		EventBus->Unsubscribe(EGameplayEventType::RewardClaimed, RewardClaimedHandle);
		EventBus->Unsubscribe(EGameplayEventType::PreparationExited, PreparationExitedHandle);
	}

	Super::Deinitialize();
}

void URunFlowWorldSubsystem::Tick(float DeltaTime)
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->FlushQueuedCommands();
	}

	if (UGameplayEventBus* EventBus = GetEventBus())
	{
		EventBus->FlushQueuedEvents();
	}
}

TStatId URunFlowWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URunFlowWorldSubsystem, STATGROUP_Tickables);
}

void URunFlowWorldSubsystem::HandleNodeSelected(const FGameplayEvent& Event)
{
	if (URunStateWorldSubsystem* RunState = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr)
	{
		RunState->SetPhase(ERunPhase::Combat);
	}

	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::StartEncounter;
	Command.NameValue = Event.NameValue;
	DispatchCommand(Command);
}

void URunFlowWorldSubsystem::HandleEncounterCompleted(const FGameplayEvent& Event)
{
	if (URunStateWorldSubsystem* RunState = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr)
	{
		RunState->SetPhase(ERunPhase::Reward);
	}

	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::BuildRewardOptions;
	Command.NameValue = Event.NameValue;
	DispatchCommand(Command);
}

void URunFlowWorldSubsystem::HandleRewardClaimed(const FGameplayEvent& Event)
{
	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::CompleteNode;
	Command.NameValue = Event.NameValue;
	DispatchCommand(Command);

	if (URunStateWorldSubsystem* RunState = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr)
	{
		RunState->SetPhase(ERunPhase::MapSelection);
		RunState->ClearPendingRewardOptions();
	}
}

void URunFlowWorldSubsystem::HandlePreparationExited(const FGameplayEvent& Event)
{
	FGameplayCommand Command;
	Command.Type = EGameplayCommandType::CompleteNode;
	Command.NameValue = Event.NameValue;
	DispatchCommand(Command);
}

void URunFlowWorldSubsystem::DispatchCommand(const FGameplayCommand& Command) const
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->QueueCommand(Command);
	}
}

UGameplayEventBus* URunFlowWorldSubsystem::GetEventBus() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr;
}

UGameplayCommandDispatcher* URunFlowWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}
