#include "RewardManagerWorldSubsystem.h"
#include "RewardPoolDefinition.h"
#include "../Core/GameplayCommandDispatcher.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "../Run/RunConfigDefinition.h"
#include "../Run/RunConfigWorldSubsystem.h"
#include "../Run/RunStateWorldSubsystem.h"

void URewardManagerWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		BuildRewardOptionsHandle = Dispatcher->RegisterHandler(EGameplayCommandType::BuildRewardOptions, FGameplayCommandNativeHandler::CreateUObject(this, &URewardManagerWorldSubsystem::HandleBuildRewardOptions));
		ClaimRewardHandle = Dispatcher->RegisterHandler(EGameplayCommandType::ClaimReward, FGameplayCommandNativeHandler::CreateUObject(this, &URewardManagerWorldSubsystem::HandleClaimReward));
	}
}

void URewardManagerWorldSubsystem::Deinitialize()
{
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->UnregisterHandler(EGameplayCommandType::BuildRewardOptions, BuildRewardOptionsHandle);
		Dispatcher->UnregisterHandler(EGameplayCommandType::ClaimReward, ClaimRewardHandle);
	}

	Super::Deinitialize();
}

FGameplayCommandResult URewardManagerWorldSubsystem::HandleBuildRewardOptions(const FGameplayCommand& Command)
{
	CurrentRewardOptions.Reset();
	const URewardPoolDefinition* RewardPool = ResolveRewardPool();
	if (RewardPool && RewardPool->RewardOptions.Num() > 0)
	{
		for (const FRewardOption& RewardOption : RewardPool->RewardOptions)
		{
			if (CurrentRewardOptions.Num() >= 3)
			{
				break;
			}
			CurrentRewardOptions.Add(RewardOption);
			CurrentRewardOptions.Last().OptionId = FGuid::NewGuid();
		}
	}
	else
	{
		CurrentRewardOptions = BuildFallbackRewardOptions();
	}

	TArray<FRewardSpec> RewardSpecs;
	for (const FRewardOption& RewardOption : CurrentRewardOptions)
	{
		RewardSpecs.Add(RewardOption.RewardSpec);
	}

	if (URunStateWorldSubsystem* RunState = GetWorld() ? GetWorld()->GetSubsystem<URunStateWorldSubsystem>() : nullptr)
	{
		RunState->SetPendingRewardOptions(RewardSpecs);
	}

	BroadcastRewardOptionsBuilt();
	return FGameplayCommandResult::Success();
}

FGameplayCommandResult URewardManagerWorldSubsystem::HandleClaimReward(const FGameplayCommand& Command)
{
	if (!CurrentRewardOptions.IsValidIndex(Command.IntValue))
	{
		return FGameplayCommandResult::Failure(NSLOCTEXT("SpriteAssemble", "InvalidRewardOption", "Reward option is invalid."));
	}

	const FRewardOption RewardOption = CurrentRewardOptions[Command.IntValue];
	if (UGameplayCommandDispatcher* Dispatcher = GetCommandDispatcher())
	{
		Dispatcher->QueueCommand(BuildApplyCommand(RewardOption, Command.SourceActor.Get()));
	}

	BroadcastRewardClaimed(RewardOption);
	CurrentRewardOptions.Reset();
	return FGameplayCommandResult::Success();
}

URewardPoolDefinition* URewardManagerWorldSubsystem::ResolveRewardPool() const
{
	if (const URunConfigWorldSubsystem* RunConfig = GetWorld() ? GetWorld()->GetSubsystem<URunConfigWorldSubsystem>() : nullptr)
	{
		if (URewardPoolDefinition* RewardPool = RunConfig->FindConfigObject<URewardPoolDefinition>(SpriteAssembleRunConfigKeys::RewardPool))
		{
			return RewardPool;
		}
	}

	return RewardPoolOverride;
}

TArray<FRewardOption> URewardManagerWorldSubsystem::BuildFallbackRewardOptions() const
{
	TArray<FRewardOption> RewardOptions;

	FRewardOption SmallRedCore;
	SmallRedCore.OptionId = FGuid::NewGuid();
	SmallRedCore.ApplyType = ERewardApplyType::AddRedCore;
	SmallRedCore.RewardSpec.RewardId = TEXT("RedCoreSmall");
	SmallRedCore.RewardSpec.Quantity = 20;
	RewardOptions.Add(SmallRedCore);

	FRewardOption LargeRedCore;
	LargeRedCore.OptionId = FGuid::NewGuid();
	LargeRedCore.ApplyType = ERewardApplyType::AddRedCore;
	LargeRedCore.RewardSpec.RewardId = TEXT("RedCoreLarge");
	LargeRedCore.RewardSpec.Quantity = 40;
	RewardOptions.Add(LargeRedCore);

	FRewardOption GemSlot;
	GemSlot.OptionId = FGuid::NewGuid();
	GemSlot.ApplyType = ERewardApplyType::AddGemSlot;
	GemSlot.RewardSpec.RewardId = TEXT("GemSlot");
	GemSlot.RewardSpec.Quantity = 1;
	RewardOptions.Add(GemSlot);

	return RewardOptions;
}

FGameplayCommand URewardManagerWorldSubsystem::BuildApplyCommand(const FRewardOption& RewardOption, AActor* ClaimingActor) const
{
	FGameplayCommand Command;
	Command.SourceActor = ClaimingActor;
	Command.TargetActor = ClaimingActor;
	Command.RewardSpec = RewardOption.RewardSpec;
	Command.IntValue = RewardOption.RewardSpec.Quantity;
	Command.NameValue = RewardOption.RewardSpec.RewardId;
	Command.ObjectValue = RewardOption.PayloadObject;

	switch (RewardOption.ApplyType)
	{
	case ERewardApplyType::AddRedCore:
		Command.Type = EGameplayCommandType::AddRedCore;
		break;
	case ERewardApplyType::AddGem:
		Command.Type = EGameplayCommandType::AddGem;
		break;
	case ERewardApplyType::AddGemSlot:
		Command.Type = EGameplayCommandType::AddGemSlot;
		break;
	case ERewardApplyType::AddPotionMax:
		Command.Type = EGameplayCommandType::AddPotionMax;
		break;
	default:
		Command.Type = EGameplayCommandType::None;
		break;
	}

	return Command;
}

void URewardManagerWorldSubsystem::BroadcastRewardOptionsBuilt() const
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::RewardOptionsBuilt);
		Event.NameValue = TEXT("RewardOptionsBuilt");
		Event.IntValue = CurrentRewardOptions.Num();
		EventBus->QueueEvent(Event);
	}
}

void URewardManagerWorldSubsystem::BroadcastRewardClaimed(const FRewardOption& RewardOption) const
{
	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::RewardClaimed);
		Event.NameValue = RewardOption.RewardSpec.RewardId;
		Event.RewardSpec = RewardOption.RewardSpec;
		EventBus->QueueEvent(Event);
	}
}

UGameplayCommandDispatcher* URewardManagerWorldSubsystem::GetCommandDispatcher() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UGameplayCommandDispatcher>() : nullptr;
}
