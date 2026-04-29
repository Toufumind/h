#include "RunStateWorldSubsystem.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"

void URunStateWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		AddRedCoreHandle = Dispatcher->RegisterHandler(EGameplayCommandType::AddRedCore, FGameplayCommandNativeHandler::CreateUObject(this, &URunStateWorldSubsystem::HandleAddRedCore));
		SpendRedCoreHandle = Dispatcher->RegisterHandler(EGameplayCommandType::SpendRedCore, FGameplayCommandNativeHandler::CreateUObject(this, &URunStateWorldSubsystem::HandleSpendRedCore));
		AddGemSlotHandle = Dispatcher->RegisterHandler(EGameplayCommandType::AddGemSlot, FGameplayCommandNativeHandler::CreateUObject(this, &URunStateWorldSubsystem::HandleAddGemSlot));
		AddPotionMaxHandle = Dispatcher->RegisterHandler(EGameplayCommandType::AddPotionMax, FGameplayCommandNativeHandler::CreateUObject(this, &URunStateWorldSubsystem::HandleAddPotionMax));
		CompleteNodeHandle = Dispatcher->RegisterHandler(EGameplayCommandType::CompleteNode, FGameplayCommandNativeHandler::CreateUObject(this, &URunStateWorldSubsystem::HandleCompleteNode));
	}
}

void URunStateWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::AddRedCore, AddRedCoreHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::SpendRedCore, SpendRedCoreHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::AddGemSlot, AddGemSlotHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::AddPotionMax, AddPotionMaxHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::CompleteNode, CompleteNodeHandle);
	}

	Super::Deinitialize();
}

void URunStateWorldSubsystem::SetPhase(ERunPhase NewPhase)
{
	RunState.CurrentPhase = NewPhase;
}

void URunStateWorldSubsystem::SetPendingRewardOptions(const TArray<FRewardSpec>& RewardOptions)
{
	RunState.PendingRewardOptions = RewardOptions;
}

void URunStateWorldSubsystem::ClearPendingRewardOptions()
{
	RunState.PendingRewardOptions.Reset();
}

FGameplayCommandResult URunStateWorldSubsystem::HandleAddRedCore(const FGameplayCommand& Command)
{
	RunState.RedCore += FMath::Max(0, Command.IntValue);
	BroadcastCurrencyChanged();
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult URunStateWorldSubsystem::HandleSpendRedCore(const FGameplayCommand& Command)
{
	const int32 SpendAmount = FMath::Max(0, Command.IntValue);
	if (RunState.RedCore < SpendAmount)
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "NotEnoughRedCore", "Not enough RedCore."));
	}

	RunState.RedCore -= SpendAmount;
	BroadcastCurrencyChanged();
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult URunStateWorldSubsystem::HandleAddGemSlot(const FGameplayCommand& Command)
{
	RunState.GemSlotCount += FMath::Max(1, Command.IntValue);
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult URunStateWorldSubsystem::HandleAddPotionMax(const FGameplayCommand& Command)
{
	RunState.PotionMax += FMath::Max(1, Command.IntValue);
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult URunStateWorldSubsystem::HandleCompleteNode(const FGameplayCommand& Command)
{
	++RunState.CompletedNodeCount;
	BroadcastNodeCompleted(Command);
	return FGameplayCommandResult::Success();
}

UGameplayCommandDispatcher* URunStateWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}

void URunStateWorldSubsystem::BroadcastCurrencyChanged() const
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::RunCurrencyChanged);
		Event.IntValue = RunState.RedCore;
		EventBus->QueueEvent(Event);
	}
}

void URunStateWorldSubsystem::BroadcastNodeCompleted(const FGameplayCommand& Command) const
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::NodeCompleted);
		Event.NameValue = Command.NameValue;
		Event.IntValue = RunState.CompletedNodeCount;
		EventBus->QueueEvent(Event);
	}
}
