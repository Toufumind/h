#include "RewardViewModelWorldSubsystem.h"
#include "../Reward/RewardManagerWorldSubsystem.h"

TArray<FRewardOptionViewSnapshot> URewardViewModelWorldSubsystem::GetRewardOptions() const
{
	TArray<FRewardOptionViewSnapshot> Snapshots;
	const URewardManagerWorldSubsystem* RewardManager = GetWorld() ? GetWorld()->GetSubsystem<URewardManagerWorldSubsystem>() : nullptr;
	if (!RewardManager)
	{
		return Snapshots;
	}

	const TArray<FRewardOption>& RewardOptions = RewardManager->GetCurrentRewardOptions();
	for (int32 Index = 0; Index < RewardOptions.Num(); ++Index)
	{
		FRewardOptionViewSnapshot Snapshot;
		Snapshot.OptionIndex = Index;
		Snapshot.RewardSpec = RewardOptions[Index].RewardSpec;
		Snapshots.Add(Snapshot);
	}

	return Snapshots;
}
